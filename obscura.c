#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "types.h"

int write_response(char *page, int status, struct MHD_Connection *connection, enum MHD_ResponseMemoryMode mode);

void add_book_to_shelf2 (void *);
bool delete_book_from_shelf2(void *);
void write_response2 (void *);
void get_book_from_shelf2(void *);
void get_book_from_shelf3(void *);
struct vm *new_vm();
void parse_lines(struct vm *v, char **lines);

int book_create HANDLER {

  struct connection_context *context = *con_cls;
  struct vm *v = new_vm();
  char *u, *b;
  u = malloc(BUFFER_SIZE);
  b = malloc(BUFFER_SIZE);
  strncpy(u, url+6, BUFFER_SIZE); // +6 to strip off leading /book/
  strcpy(b, context->buffer);
  // v->buffers['0'] = page; -- reserved
  v->buffers['5'] = u;
  v->buffers['6'] = b;
  v->pointers['3'] = connection;
  v->pointers['9'] = &add_book_to_shelf2;
  v->pointers['A'] = &write_response2;
  // v->pointers['b'] = isbn; -- reserved
  // v->pointers['c'] = title; -- reserved
  v->loops['j'] = context->buffer_size+1;

  char *lines[] = {
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
    "x A a",                         // execute function A with pointer a (and 7, 3, 4)
    NULL
  };

  parse_lines(v, lines);

  return *(int*)(void **)(v->pointers + '5');
}

int book_read HANDLER {

  struct connection_context *context = *con_cls;
  struct vm *v = new_vm();
  char *u;
  u = malloc(BUFFER_SIZE);
  strncpy(u, url+6, BUFFER_SIZE); // +6 to strip off leading /book/
  // v->buffers['0'] = page; -- reserved
  v->buffers['5'] = u;
  // v->buffers['7'] = character; -- reserved
  // v->buffers['8'] = return check; -- reserved
  v->pointers['3'] = connection;
  v->pointers['A'] = &write_response2;
  v->pointers['B'] = &get_book_from_shelf2;
  // v->pointers['b'] = isbn; -- reserved
  // v->pointers['c'] = title; -- reserved

  char *lines[] = {
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
    "x A a",                       // execute function A with pointer a (and 7, 3, 4)
    NULL
  };

  parse_lines(v, lines);

  return *(int*)(void **)(v->pointers + '5');
}

int book_update HANDLER {
  struct connection_context *context = *con_cls;
  struct vm *v = new_vm();
  char *u, *b;
  u = malloc(BUFFER_SIZE);
  b = malloc(BUFFER_SIZE);
  strncpy(u, url+6, BUFFER_SIZE); // +6 to strip off leading /book/
  strcpy(b, context->buffer);
  // v->buffers['0'] = page; -- reserved
  v->buffers['5'] = u;
  v->buffers['6'] = b;
  // v->buffers['7'] = character; -- reserved
  // v->buffers['8'] = return check; -- reserved
  v->pointers['3'] = connection;
  v->pointers['A'] = &write_response2;
  v->pointers['C'] = &get_book_from_shelf3;
  // v->pointers['b'] = isbn; -- reserved
  // v->pointers['c'] = title; -- reserved

  char *lines[] = {
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
    "x A a",                       // execute function A with pointer a (and 7, 3, 4)
    NULL
  };
  parse_lines(v, lines);

  return *(int*)(void **)(v->pointers + '5');
}

int book_delete HANDLER {
  struct connection_context *context = *con_cls;
  struct vm *v = new_vm();
  char *u;
  u = malloc(BUFFER_SIZE);
  strncpy(u, url+6, BUFFER_SIZE); // +6 to strip off leading /book/
  // v->buffers['0'] = page; -- reserved
  v->buffers['5'] = u;
  // v->buffers['7'] = character; -- reserved
  // v->buffers['8'] = return check; -- reserved
  v->pointers['3'] = connection;
  v->pointers['A'] = &write_response2;
  v->pointers['D'] = &delete_book_from_shelf2;
  // v->pointers['b'] = isbn; -- reserved

  char *lines[] = {
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
    "x A a",                       // execute function A with pointer a (and 7, 3, 4)
    NULL
  };
  parse_lines(v, lines);

  return *(int*)(void **)(v->pointers + '5');
}
