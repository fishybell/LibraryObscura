#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

#include "types.h"

#define GET             0
#define POST            1

static struct http_route *global_routes;

struct vm *new_vm();
void parse_lines(struct vm *v, char **lines);

void create_response_from_buffer(void *ptr) {
  printf("4 ptr address: %p\n", ptr);
  int mode = *(int*)*(void**)(ptr);
  printf("mode? %d\n", mode);


  ptr += ('a' - '4') * (sizeof(void*));
  printf("a ptr address: %p\n", ptr);
  char *page = (char*)*(void**)(ptr);
  printf("page? %s\n", page);

  ptr -= ('a' - '6') * (sizeof(void*));
  printf("6 ptr address: %p\n", ptr);
  fflush(stdout);

  void * p = MHD_create_response_from_buffer(strlen(page), (void*)page, mode);
  printf("p address: %p\n", p);
  *(void **)ptr = p;
  printf("ptr address: %p\n", *(void**)(ptr));
}

void queue_response(void *ptr) {
}

void destroy_response(void *ptr) {
}

int write_response(char *page, int status, struct MHD_Connection *connection, enum MHD_ResponseMemoryMode mode) {
  struct MHD_Response *response;
  int ret;

  struct vm *v = new_vm();
  v->buffers['0'] = page;
  v->pointers['0'] = &create_response_from_buffer;
  v->pointers['1'] = &queue_response;
  v->pointers['2'] = &destroy_response;
  v->pointers['3'] = connection;
  v->pointers['4'] = &mode;
  v->pointers['5'] = &ret;
  v->pointers['6'] = &response;
  printf("original mode %d\n", mode);
  printf("v->pointers['a'] address: %p\n", v->pointers + 'a');
  printf("v->pointers['4'] address: %p\n", v->pointers + '4');
  printf("v->pointers['6'] address: %p\n", v->pointers + '6');

  char *lines[] = {
    // get response from buffer
    "u a 0",                       // point pointer a to buffer 0
    "x 0 4",                       // execute function 0 with pointer 4
    NULL
  };

  parse_lines(v, lines);

  printf("Responding with %lu bytes\n", strlen(page));

	// response = MHD_create_response_from_buffer (strlen (page), (void*) page, mode);
  response = (struct MHD_Response *)*(void**)(v->pointers + '6');
  printf("response address: %p\n", response);

  ret = MHD_queue_response (connection, status, response);
  MHD_destroy_response (response);

  return ret;
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
  int t;
  regex_t re;
  char    buffer[BUFFER_SIZE];

  if ((t=regcomp( &re, route.path_regex, REG_NOSUB )) != 0) {
    regerror(t, &re, buffer, sizeof buffer);
    fprintf(stderr,"Invalid route regex: %s (%s)\n", buffer, route.path_regex);

    exit(1);
  }

  if (strncmp(route.method, method, strlen(route.method)) == 0 &&
      regexec( &re, url, 0, NULL, 0 ) == 0) {
    return true;
  }

  return false;
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
