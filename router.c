#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "types.h"

#define GET             0
#define POST            1

handler_func list_read, book_create, book_read, book_update, book_delete, not_found_handler;
struct vm *new_vm();
void parse_lines(struct vm *v, char **lines);

void create_response_from_buffer(void *ptr) {
printf("got to create_response_from_buffer\n");
  int mode = *(int*)PTR;
printf("mode %d\n", mode);
  MPTR('a', '4');
  char *page = (char*)PTR;
printf("page %s\n", page);
  MPTR('6', 'a');
  *(void **)ptr = MHD_create_response_from_buffer(strlen(page), (void*)page, mode);
printf("finished create_response_from_buffer\n");
}

void queue_response(void *ptr) {
printf("got to queue_response\n");
  struct MHD_Connection *connection = (struct MHD_Connection *)PTR;
printf("connection %p\n", connection);
  MPTR('6', '3');
  struct MHD_Response *response = (struct MHD_Response *)PTR;
printf("response %p\n", response);
  MPTR('7', '6');
  int status = *(int*)PTR;
printf("status %d\n", status);
  MPTR('5', '7');
  *(int*)(void **)ptr = MHD_queue_response (connection, status, response);
printf("finished queue_response\n");
}

void destroy_response(void *ptr) {
printf("got to destroy_response\n");
  struct MHD_Response *response = (struct MHD_Response *)PTR;
printf("response %p\n", response);
  MHD_destroy_response (response);
printf("finished destroy_response\n");
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

int not_found_handler HANDLER {
  return write_response("Route not found", MHD_HTTP_NOT_FOUND, connection, MHD_RESPMEM_PERSISTENT);
}

void
request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe) {
  struct connection_context *context = *con_cls;
  printf("completed?");

  if (context == NULL)
    return;

  free (context->buffer);
  free (context);
  *con_cls = NULL;
}

bool route_matches(struct http_route route, const char *url, const char *method) {
  struct vm *v = new_vm();
  char u[BUFFER_SIZE], m[BUFFER_SIZE];
  strcpy(u, url);
  strcpy(m, method);
  v->buffers['1'] = route.method;
  v->buffers['2'] = m;
  v->buffers['3'] = route.path;
  v->buffers['4'] = u;

  char *lines[] = {
    // match url and method, if they both match, move to end
    "r c *",                       // set register c to 42
    "l j ,",                       // set loop iterator i to -4 (so I can arbitrarily jump multiple times)
    "m 1 2 3",                     // match buffer 1 to buffer 2, or jump forward 3
    "m 3 4 2",                     // match buffer 3 to buffer 4, or jump forward 2
    "e j 0 -",                     // increment loop iterator i, check against 0, jump backwards -3 ('0' - '-' == -3)

    // or make the return value wrong
    "l i 0",                       // initialize loop incrementor i to 0
    "e j 0 .",                     // increment loop iterator i, check against 0, jump backwards -2 ('0' - '.' == -2)

    // make the ruturn value right
    "l i 1",                       // initialize loop incrementor i to 1
    "a c c i",                     // add loop incremenetor i to c
    "o c 8",                       // store register c in pointer 8 as a int* instead of a void*
    NULL
  };

  parse_lines(v, lines);

  return *(int*)(void **)(v->pointers['8']) - 42;
}

int route_to_handler HANDLER {

  struct connection_context *context;
  int i;

  static struct http_route routes[6] = {
    (struct http_route){
      "GET",
      "/list",
      &list_read,
    },
    (struct http_route){
      "POST",
      "/book/",
      &book_create,
    },
    (struct http_route){
      "GET",
      "/book/",
      &book_read,
    },
    (struct http_route){
      "PATCH",
      "/book/",
      &book_update,
    },
    (struct http_route){
      "DELETE",
      "/book/",
      &book_delete,
    },
    (struct http_route){
      "",
      "",
      &not_found_handler,
    }
  };


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
  while(routes[i].func != NULL) {
    if (route_matches(routes[i], url, method)) {
    printf("calling route %d\n", i);
      int ret = routes[i].func(cls, connection, url, method, version, context->buffer, &context->buffer_size, con_cls);

    printf("returning %d\n", ret);
    return ret;
    }

    i++;
  }

  fprintf(stderr, "No route defined, disconnecting\n");

  return MHD_NO;
}
