#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "types.h"

#define MIN(A, B)  ((A) < (B) ? (A) : (B))

int write_response(char *page, int status, struct MHD_Connection *connection, enum MHD_ResponseMemoryMode mode);
void add_book_to_shelf2 (void *);
void write_response2 (void *);
struct book *get_book_from_shelf(char *isbn);
bool delete_book_from_shelf(char *isbn);
struct vm *new_vm();
void parse_lines(struct vm *v, char **lines);

int book_create (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {

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
    // call add_book_to_shelf
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

  //  //char *url_copy = malloc(sizeof(char) * BUFFER_SIZE);
  //  //char *body_copy = malloc(sizeof(char) * MIN(BUFFER_SIZE, context->buffer_size+1)); // allow for null terminator

  //  //strncpy(url_copy, url+6, BUFFER_SIZE); // +6 to strip off leading /book/
  //  //strncpy(body_copy, context->buffer, MIN(BUFFER_SIZE, context->buffer_size)); // +6 to strip off leading /book/
  //body_copy[context->buffer_size] = 0;

  //add_book_to_shelf((struct book){url_copy, body_copy});
  //return write_response("Created book", MHD_HTTP_OK, connection, MHD_RESPMEM_PERSISTENT);
}

int book_read (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
  strncpy(buffer, url+6, BUFFER_SIZE); // +6 to strip off leading /book/

  struct book *item = get_book_from_shelf(buffer);

  if (item == NULL) {
    return write_response("Book not found\n", MHD_HTTP_NOT_FOUND, connection, MHD_RESPMEM_PERSISTENT);
  }

  // re-use buffer
  sprintf(buffer, "%s: %s\n", item->isbn, item->title);

  return write_response(buffer, MHD_HTTP_OK, connection, MHD_RESPMEM_MUST_FREE);
}

int book_update (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  struct connection_context *context = *con_cls;
  char *body_copy;
  char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
  strncpy(buffer, url+6, BUFFER_SIZE); // +6 to strip off leading /book/

  struct book *item = get_book_from_shelf(buffer);

  free(buffer);

  if (item == NULL) {
    return write_response("Book not found\n", MHD_HTTP_NOT_FOUND, connection, MHD_RESPMEM_PERSISTENT);
  }

  body_copy = malloc(sizeof(char) * MIN(BUFFER_SIZE, context->buffer_size+1)); // allow for null terminator
  strncpy(body_copy, context->buffer, MIN(BUFFER_SIZE, context->buffer_size)); // +6 to strip off leading /book/
  body_copy[context->buffer_size] = 0;
  free(item->title);
  item->title = body_copy;

  return write_response("Book updated\n", MHD_HTTP_OK, connection, MHD_RESPMEM_PERSISTENT);
}

int book_delete (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  struct connection_context *context = *con_cls;
  char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
  bool found;
  strncpy(buffer, url+6, BUFFER_SIZE); // +6 to strip off leading /book/

  found = delete_book_from_shelf(buffer);

  free(buffer);

  if (!found) {
    return write_response("Book not found\n", MHD_HTTP_NOT_FOUND, connection, MHD_RESPMEM_PERSISTENT);
  }

  return write_response("Book deleted\n", MHD_HTTP_OK, connection, MHD_RESPMEM_PERSISTENT);
}
