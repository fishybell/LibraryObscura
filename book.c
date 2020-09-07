#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "types.h"

#define MIN(A, B)  ((A) < (B) ? (A) : (B))

int write_response(char *page, int status, struct MHD_Connection *connection, enum MHD_ResponseMemoryMode mode);
bool delete_book_from_shelf(char *isbn);

//int book_update (void *cls, struct MHD_Connection *connection,
//                          const char *url,
//                          const char *method, const char *version,
//                          const char *upload_data,
//                          size_t *upload_data_size, void **con_cls) {
//  struct connection_context *context = *con_cls;
//  char *body_copy;
//  char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
//  strncpy(buffer, url+6, BUFFER_SIZE); // +6 to strip off leading /book/
//
//  struct book *item = get_book_from_shelf(buffer);
//
//  free(buffer);
//
//  if (item == NULL) {
//    return write_response("Book not found\n", MHD_HTTP_NOT_FOUND, connection, MHD_RESPMEM_PERSISTENT);
//  }
//
//  body_copy = malloc(sizeof(char) * MIN(BUFFER_SIZE, context->buffer_size+1)); // allow for null terminator
//  strncpy(body_copy, context->buffer, MIN(BUFFER_SIZE, context->buffer_size)); // +6 to strip off leading /book/
//  body_copy[context->buffer_size] = 0;
//  free(item->title);
//  item->title = body_copy;
//
//  return write_response("Book updated\n", MHD_HTTP_OK, connection, MHD_RESPMEM_PERSISTENT);
//}

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
