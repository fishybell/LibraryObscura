#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "types.h"

#define GET             0
#define POST            1

void parse_lines(struct vm *v, char **lines);
void list_read2 (void *);
void add_book_to_shelf2 (void *);
bool delete_book_from_shelf2(void *);
void get_book_from_shelf2(void *);
void get_book_from_shelf3(void *);

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

int route_to_handler HANDLER {

  struct connection_context *context;
  int i, ret;
  struct vm *v = malloc(sizeof(struct vm));
  char *u, *b, *m;

  static struct http_route routes[] = {
    (struct http_route){
      "GET",
      "/list",
      // &list_read,
      // { NULL },
      {
        // call list_read with correct parameters
        "x A a",                         // execute function 9 with pointer b (and c)

        // status
        "r c d",                       // set register c to 100
        "l i b",                       // set loop incrementor i to 50
        "a c c i",                     // add i to c (150)

        // jump and jump (will hit one of these two only)
        "e j 0 .",                     // increment loop iterator j, check against 0, jump backwards -2 ('0' - '.' == -2)
        "e j 0 #",                     // increment loop iterator j, check against 0, jump backwards -13 ('0' - '#' == -13)

        "a c c i",                     // add i to c (200)
        "o c 7",                       // store register c in pointer 7 as a int* instead of a void*

        // mode
        "l j 1",                       // set j to 1
        "a z z j",                     // add j to z (1) (free memory)
        "o z 4",                       // store register z in pointer 4 as a int* instead of a void* (skip to here from above)

        // execute
        "x 0 4",                       // execute function 0 with pointer 4 (and 6 and a)
        "x 1 3",                       // execute function 1 with pointer 3 (and 6, 7, and 5)
        "x 2 6",                       // execute function 2 with pointer 6
        NULL
      },
    },
    (struct http_route){
      "POST",
      "/book/",
      {
        // call add_book_to_shelf with correct parameters
        "r c 2",                         // set register c to 50
        "l i &",                         // set loop incrementor i to -10
        "a c c i",                       // add i to c (40)
        "a c c i",                       // add i to c (30)
        "a c c i",                       // add i to c (20)
        "a c c i",                       // add i to c (10)
        "a z c i",                       // add i to z (0)
        "i 6 j z",                       // null terminate buffer 6
        "u b 5",                         // pointers b is buffer 5
        "u c 6",                         // pointers c is buffer 6
        "x 9 b",                         // execute function 9 with pointer b (and c)
        "b 0 Created book\n",            // set buffer 0
        "u a 0",                         // point pointer a to buffer 0
        "r c d",                         // set register c to 100
        "l i b",                         // set loop incrementor i to 50
        "a c c i",                       // add i to c (150)
        "a c c i",                       // add i to c (200)
        "o c 7",                         // store register c in pointer 7 as a int* instead of a void*
        "o z 4",                         // store register c in pointer 7 as a int* instead of a void*
        "x 0 4",                       // execute function 0 with pointer 4 (and 6 and a)
        "x 1 3",                       // execute function 1 with pointer 3 (and 6, 7, and 5)
        "x 2 6",                       // execute function 2 with pointer 6
        NULL
      },
    },
    (struct http_route){
      "GET",
      "/book/",
      {
        // call get_book_from_shelf with correct parameters
        "u b 5",                       // pointers b is buffer 5
        "x B b",                       // execute function 9 with pointer b
        "b 0 Book not found\n",        // allocate buffer 0
        "b 7 %",                       // allocate buffer 7
        "t 8 d",                       // copy pointer d to buffer 8
        "r c 2",                       // set register c to 50
        "l i &",                       // set loop incrementor i to -10
        "a c c i",                     // add i to c (40)
        "a c c i",                     // add i to c (30)
        "a c c i",                     // add i to c (20)
        "a c c i",                     // add i to c (10)
        "a z c i",                     // add i to z (0)

        // get values back out
        // not found?
        "m 7 8 >",                     // match buffer 7 to buffer 8, or jump forward 14
        "l j ,",                       // set loop iterator i to -4 (so I can arbitrarily jump multiple times)
        "r c d",                       // set register c to 100
        "l i b",                       // set loop incrementor i to 50
        "a c c i",                     // add i to c (150)
        "a c c i",                     // add i to c (200)
        "a c c i",                     // add i to c (250)
        "a c c i",                     // add i to c (300)
        "a c c i",                     // add i to c (350)
        "a c c i",                     // add i to c (400)
        "l i 4",                       // set loop incrementor i to 4
        "a c c i",                     // add i to c (404)
        "o c 7",                       // store register c in pointer 7 as a int* instead of a void*
        "e j 0  ",                     // increment loop iterator j, check against 0, jump backwards -16 ('0' - ' ' == -16) (maximum forward jump)

        // page init
        "l k 0",                       // set k to 0
        "i 0 k z",                     // null terminate buffer 6
        // add isbn buffer to buffer
        "t 8 b",                       // copy pointer b to buffer 8
        "g 0 8",
        // add ':'
        "b 7 :",
        "g 0 7",

        // add ' '
        "b 7  ",
        "g 0 7",

        // add title buffer to buffer
        "t 8 c",                       // copy pointer c to buffer 8
        "g 0 8",

        // add '\n'
        "b 7 \n",
        "g 0 7",

        // status
        "r c d",                       // set register c to 100
        "l i b",                       // set loop incrementor i to 50
        "a c c i",                     // add i to c (150)

        // jump and jump (will hit one of these two only)
        "e j 0 .",                     // increment loop iterator j, check against 0, jump backwards -2 ('0' - '.' == -2)
        "e j 0 #",                     // increment loop iterator j, check against 0, jump backwards -13 ('0' - '#' == -13)

        "a c c i",                     // add i to c (200)
        "o c 7",                       // store register c in pointer 7 as a int* instead of a void*

        // mode
        "l j 1",                       // set j to 1
        "a z z j",                     // add j to z (1) (free memory)
        "o z 4",                       // store register z in pointer 4 as a int* instead of a void* (skip to here from above)
        "u a 0",                       // point pointer a to buffer 0

        // execute
        "x 0 4",                       // execute function 0 with pointer 4 (and 6 and a)
        "x 1 3",                       // execute function 1 with pointer 3 (and 6, 7, and 5)
        "x 2 6",                       // execute function 2 with pointer 6
        NULL
      },
    },
    (struct http_route){
      "PATCH",
      "/book/",
      {
        // call get_book_from_shelf with correct parameters
        "u b 5",                       // pointers b is buffer 5
        "x C b",                       // execute function 9 with pointer b (and c)
        "b 0 Book not found\n",        // allocate buffer 0
        "b 7 %",                       // allocate buffer 7
        "t 8 d",                       // copy pointer d to buffer 8
        "r c 2",                       // set register c to 50
        "l i &",                       // set loop incrementor i to -10
        "a c c i",                     // add i to c (40)
        "a c c i",                     // add i to c (30)
        "a c c i",                     // add i to c (20)
        "a c c i",                     // add i to c (10)
        "a z c i",                     // add i to z (0)

        // get values back out
        // not found?
        "m 7 8 >",                     // match buffer 7 to buffer 8, or jump forward 14
        "l j ,",                       // set loop iterator i to -4 (so I can arbitrarily jump multiple times)
        "r c d",                       // set register c to 100
        "l i b",                       // set loop incrementor i to 50
        "a c c i",                     // add i to c (150)
        "a c c i",                     // add i to c (200)
        "a c c i",                     // add i to c (250)
        "a c c i",                     // add i to c (300)
        "a c c i",                     // add i to c (350)
        "a c c i",                     // add i to c (400)
        "l i 4",                       // set loop incrementor i to 4
        "a c c i",                     // add i to c (404)
        "o c 7",                       // store register c in pointer 7 as a int* instead of a void*
        "e j 0 %",                     // increment loop iterator j, check against 0, jump backwards -16 ('0' - ' ' == -16) (maximum forward jump)

        // reset isbn and title
        "s b 5",                       // swap pointer at b for buffer 5
        "s c 6",                       // swap pointer at c for buffer 6

        // page init
        "b 0 Book updated\n",          // setup buffer 0

        // status
        "r c d",                       // set register c to 100
        "l i b",                       // set loop incrementor i to 50
        "a c c i",                     // add i to c (150)
        "a c c i",                     // add i to c (200)
        "o c 7",                       // store register c in pointer 7 as a int* instead of a void*

        // mode
        "l j 1",                       // set j to 1
        "a z z j",                     // add j to z (1) (free memory)
        "o z 4",                       // store register z in pointer 4 as a int* instead of a void* (skip to here from above)
        "u a 0",                       // point pointer a to buffer 0

        // execute
        "x 0 4",                       // execute function 0 with pointer 4 (and 6 and a)
        "x 1 3",                       // execute function 1 with pointer 3 (and 6, 7, and 5)
        "x 2 6",                       // execute function 2 with pointer 6
        NULL
      },
    },
    (struct http_route){
      "DELETE",
      "/book/",
      {
        // call delete_book_from_shelf with correct parameters
        "u b 5",                       // pointers b is buffer 5
        "x D b",                       // execute function 9 with pointer b (and c)
        "b 0 Book not found\n",        // allocate buffer 0
        "b 7 %",                       // allocate buffer 7
        "t 8 d",                       // copy pointer d to buffer 8
        "r c 2",                       // set register c to 50
        "l i &",                       // set loop incrementor i to -10
        "a c c i",                     // add i to c (40)
        "a c c i",                     // add i to c (30)
        "a c c i",                     // add i to c (20)
        "a c c i",                     // add i to c (10)
        "a z c i",                     // add i to z (0)

        // get values back out
        // not found?
        "m 7 8 >",                     // match buffer 7 to buffer 8, or jump forward 14
        "l j ,",                       // set loop iterator i to -4 (so I can arbitrarily jump multiple times)
        "r c d",                       // set register c to 100
        "l i b",                       // set loop incrementor i to 50
        "a c c i",                     // add i to c (150)
        "a c c i",                     // add i to c (200)
        "a c c i",                     // add i to c (250)
        "a c c i",                     // add i to c (300)
        "a c c i",                     // add i to c (350)
        "a c c i",                     // add i to c (400)
        "l i 4",                       // set loop incrementor i to 4
        "a c c i",                     // add i to c (404)
        "o c 7",                       // store register c in pointer 7 as a int* instead of a void*
        "e j 0 '",                     // increment loop iterator j, check against 0, jump backwards -16 ('0' - ' ' == -16) (maximum forward jump)

        // page init
        "b 0 Book deleted\n",          // setup buffer 0

        // status
        "r c d",                       // set register c to 100
        "l i b",                       // set loop incrementor i to 50
        "a c c i",                     // add i to c (150)
        "a c c i",                     // add i to c (200)
        "o c 7",                       // store register c in pointer 7 as a int* instead of a void*

        // mode
        "l j 1",                       // set j to 1
        "a z z j",                     // add j to z (1) (free memory)
        "o z 4",                       // store register z in pointer 4 as a int* instead of a void* (skip to here from above)
        "u a 0",                       // point pointer a to buffer 0

        // execute
        "x 0 4",                       // execute function 0 with pointer 4 (and 6 and a)
        "x 1 3",                       // execute function 1 with pointer 3 (and 6, 7, and 5)
        "x 2 6",                       // execute function 2 with pointer 6
        NULL
      },
    },
    (struct http_route){
      "",
      "",
      {
        "r c d",                       // set register c to 100
        "l i b",                       // set loop incrementor i to 50
        "a c c i",                     // add i to c (150)
        "a c c i",                     // add i to c (200)
        "a c c i",                     // add i to c (250)
        "a c c i",                     // add i to c (300)
        "a c c i",                     // add i to c (350)
        "a c c i",                     // add i to c (400)
        "l i 4",                       // set loop incrementor i to 4
        "a c c i",                     // add i to c (404)
        "o c 7",                       // store register c in pointer 7 as a int* instead of a void*
        "o z 4",                       // store register z in pointer 4 as a int* instead of a void* (skip to here from above)
        "u a 9",                       // point pointer a to buffer 0
        "x 0 4",                       // execute function 0 with pointer 4 (and 6 and a)
        "x 1 3",                       // execute function 1 with pointer 3 (and 6, 7, and 5)
        "x 2 6",                       // execute function 2 with pointer 6
        NULL
      },
    },
    (struct http_route){
      "",
      "",
      {
        // match url and method, if they both match, move to end
        "r c *",                       // set register c to 42
        "l j ,",                       // set loop iterator i to -4 (so I can arbitrarily jump multiple times)
        "m A B 3",                     // match buffer A to buffer B, or jump forward 3
        "m C 5 2",                     // match buffer C to buffer 5, or jump forward 2
        "e j 0 -",                     // increment loop iterator i, check against 0, jump backwards -3 ('0' - '-' == -3)

        // or make the return value wrong
        "l i 0",                       // initialize loop incrementor i to 0
        "e j 0 .",                     // increment loop iterator i, check against 0, jump backwards -2 ('0' - '.' == -2)

        // make the ruturn value right
        "l i 1",                       // initialize loop incrementor i to 1
        "a c c i",                     // add loop incremenetor i to c
        "o c 5",                       // store register c in pointer 5 as a int* instead of a void*
        NULL
      }
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

  i = 0;
  u = malloc(BUFFER_SIZE);
  b = malloc(BUFFER_SIZE);
  m = malloc(BUFFER_SIZE);
  strncpy(u, url+6, BUFFER_SIZE); // +6 to strip off leading /book/
  strcpy(b, context->buffer);
  strcpy(u, url);
  strcpy(m, method);
  // v->buffers['0'] = page; -- reserved
  v->buffers['5'] = u;
  v->buffers['6'] = b;
  v->buffers['9'] = "Route not found\n";
  v->buffers['B'] = m;
  v->pointers['0'] = &create_response_from_buffer;
  v->pointers['1'] = &queue_response;
  v->pointers['2'] = &destroy_response;
  v->pointers['3'] = connection;
  v->pointers['9'] = &add_book_to_shelf2;
  v->pointers['A'] = &list_read2;
  v->pointers['B'] = &get_book_from_shelf2;
  v->pointers['C'] = &get_book_from_shelf3;
  v->pointers['D'] = &delete_book_from_shelf2;
  v->loops['j'] = context->buffer_size+1;

  if (context->too_big) {
    v->buffers['9'] = "Title too long\n";
    parse_lines(v, routes[5].lines);
    ret = *(int*)(void **)(v->pointers + '5');
    v->buffers['9'] = "Route not found\n";

    return ret;
  }

  for(;;i++) {
    v->buffers['A'] = routes[i].method;
    v->buffers['C'] = routes[i].path;
    parse_lines(v, routes[6].lines);
    ret = *(int*)(void **)(v->pointers['5']);
    if (ret - 42) {
      parse_lines(v, routes[i].lines);
      return *(int*)(void **)(v->pointers + '5');
    }
  }

  fprintf(stderr, "No route defined, disconnecting\n");

  return MHD_NO;
}
