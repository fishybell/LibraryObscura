#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "types.h"

#define GET             0
#define POST            1

static struct http_route *global_routes;

struct vm *new_vm();
void parse_lines(struct vm *v, char **lines);

void create_response_from_buffer(void *ptr) {
  int mode = *(int*)PTR;
  MPTR('a', '4');
  char *page = (char*)PTR;
  MPTR('6', 'a');
  *(void **)ptr = MHD_create_response_from_buffer(strlen(page), (void*)page, mode);
}

void queue_response(void *ptr) {
  struct MHD_Connection *connection = (struct MHD_Connection *)PTR;
  MPTR('6', '3');
  struct MHD_Response *response = (struct MHD_Response *)PTR;
  MPTR('7', '6');
  int status = *(int*)PTR;
  MPTR('5', '7');
  *(int*)(void **)ptr = MHD_queue_response (connection, status, response);
}

void destroy_response(void *ptr) {
  struct MHD_Response *response = (struct MHD_Response *)PTR;
  MHD_destroy_response (response);
}

int write_response(char *page, int status, struct MHD_Connection *connection, enum MHD_ResponseMemoryMode mode) {
  struct vm *v = new_vm();
  v->buffers['0'] = page;
  v->pointers['0'] = &create_response_from_buffer;
  v->pointers['1'] = &queue_response;
  v->pointers['2'] = &destroy_response;
  v->pointers['3'] = connection;
  v->pointers['4'] = &mode;
  // v->pointers['5'] = &ret; -- reserved
  // v->pointers['6'] = &response; -- reserved
  v->pointers['7'] = &status;

  char *lines[] = {
    // get response from buffer
    "u a 0",                       // point pointer a to buffer 0
    "x 0 4",                       // execute function 0 with pointer 4 (and 6 and a)
    "x 1 3",                       // execute function 1 with pointer 3 (and 6, 7, and 5)
    "x 2 6",                       // execute function 2 with pointer 6
    NULL
  };

  parse_lines(v, lines);

  printf("Responding with %lu bytes\n", strlen(page));

  return *(int*)(void **)(v->pointers + '5');
}

int not_found_handler (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  return write_response("Route not found", MHD_HTTP_NOT_FOUND, connection, MHD_RESPMEM_PERSISTENT);
}

void
request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe) {
  struct connection_context *context = *con_cls;

  if (context == NULL)
    return;

  free (context->buffer);
  free (context);
  *con_cls = NULL;
}

bool route_matches(struct http_route route, const char *url, const char *method) {
  struct vm *v = new_vm();
  char *u, *m;
  sprintf(u, "%s", url);
  sprintf(m, "%s", method);
  v->buffers['0'] = route.method;
  v->buffers['1'] = m;
  v->buffers['2'] = route.path;
  v->buffers['3'] = u;

  char *lines[] = {
    // match url and method, if they both match, move to end
    "r c *",                       // set register c to 42
    "l j ,",                       // set loop iterator i to -4 (so I can arbitrarily jump multiple times)
    "m 0 1 3",                     // match buffer 0 to buffer 1, or jump forward 3
    "m 2 3 2",                     // match buffer 0 to buffer 1, or jump forward 3
    "e j 0 ,",                     // increment loop iterator i, check against 0, jump backwards -4 ('0' - ',' == -4)

    // or make the return value wrong
    "l i 0",                       // initialize loop incrementor i to 0
    "e j 0 .",                     // increment loop iterator i, check against 0, jump backwards -2 ('0' - '.' == -2)

    // make the ruturn value right
    "l i 1",                       // initialize loop incrementor i to 1
    "o c d",                       // store register c in pointer d as a int* instead of a void*
    NULL
  };

  parse_lines(v, lines);

  return *(int*)(void **)(v->pointers + '0') - 42;
}

int route_to_handler (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {

  struct connection_context *context;
  int i;

  // create a context object around this connection first (we'll get called again with the same object)
  if (*con_cls == NULL) {

    context = malloc (sizeof (struct connection_context));
    if (context == NULL) {
      return MHD_NO;
    }

    context->buffer = malloc(BUFFER_SIZE);
    if (context->buffer == NULL) {
      return MHD_NO;
    }
    context->buffer_size = 0;
    context->too_big = false;

    *con_cls = (void *) context;

    return MHD_YES;
  }

  context = *con_cls;

  printf("url: %s\n", url);
  printf("method: %s\n", method);
  printf("bytes: %ld\n", context->buffer_size);

  if (upload_data_size != NULL) {

    if (*upload_data_size != 0) {

      if (context->buffer_size + *upload_data_size < BUFFER_SIZE) {
        memcpy(context->buffer + context->buffer_size, upload_data, *upload_data_size);
        context->buffer_size += *upload_data_size;
      } else {
        context->too_big = true;
      }

      *upload_data_size = 0;

      return MHD_YES;
    }

  }

  if (context->too_big) {
    return write_response("Title too long\n", MHD_HTTP_BAD_REQUEST, connection, MHD_RESPMEM_PERSISTENT);
  }

  i = 0;
  while(global_routes[i].func != NULL) {
    if (route_matches(global_routes[i], url, method)) {
      return global_routes[i].func(cls, connection, url, method, version, context->buffer, &context->buffer_size, con_cls);
    }

    i++;
  }

  fprintf(stderr, "No route defined, disconnecting\n");

  return MHD_NO;
}

MHD_AccessHandlerCallback router(int count, ...) {
  // Closures don't really exist in C. The standard methodolgy is to use a nested callback function
  // Unfortunately that enables an executable stack, which is discouraged
  // Instead we're breaking consistency by using a singleton object that's shared between 2 functions in this file.
  // Each time this function is called it overwrites the previous routing.
  // Long story short, don't call this function twice.

  va_list args;
  int i, sum;

  global_routes=malloc(sizeof(handler_func) * (count)+1);

  va_start (args, count);

  for (i = 0; i < count; i++) {
    global_routes[i]  = va_arg (args, struct http_route);
  }
  global_routes[i] = (struct http_route){NULL, NULL, NULL};

  va_end (args);

  return &route_to_handler;
}
