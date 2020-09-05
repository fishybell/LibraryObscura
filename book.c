#include <string.h>
#include <stdlib.h>

#include "types.h"

#define BUFFER_SIZE 256

#define MIN(A, B)  ((A) < (B) ? (A) : (B))


int write_response(char *page, int status, struct MHD_Connection *connection, enum MHD_ResponseMemoryMode mode);
void add_book_to_shelf (struct book item);

int book_create (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {

  struct connection_context *context = *con_cls;

  char *url_copy = malloc(sizeof(char) * BUFFER_SIZE);
  char *body_copy = malloc(sizeof(char) * MIN(BUFFER_SIZE, context->buffer_size+1)); // allow for null terminator

  strncpy(url_copy, url+6, BUFFER_SIZE); // +6 to strip off leading /book/
  strncpy(body_copy, context->buffer, MIN(BUFFER_SIZE, context->buffer_size)); // +6 to strip off leading /book/

  add_book_to_shelf((struct book){url_copy, body_copy});
  return write_response("Created book", MHD_HTTP_OK, connection, MHD_RESPMEM_PERSISTENT);
}

int book_read (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  return write_response("Read book", MHD_HTTP_OK, connection, MHD_RESPMEM_PERSISTENT);
}

int book_update (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  return write_response("Update book", MHD_HTTP_OK, connection, MHD_RESPMEM_PERSISTENT);
}

int book_delete (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  return write_response("Delete book", MHD_HTTP_OK, connection, MHD_RESPMEM_PERSISTENT);
}
