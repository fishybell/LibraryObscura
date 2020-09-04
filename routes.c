#include <string.h>
#include "types.h"

int write_response(char *page, int status, struct MHD_Connection *connection) {
  struct MHD_Response *response;
  int ret;

  response = MHD_create_response_from_buffer (strlen (page),
                                            (void*) page, MHD_RESPMEM_PERSISTENT);

  ret = MHD_queue_response (connection, status, response);
  MHD_destroy_response (response);

  return ret;
}

int list_read (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  return write_response("List of books", MHD_HTTP_OK, connection);
}

int book_create (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  return write_response("Create book", MHD_HTTP_OK, connection);
}

int book_read (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  return write_response("Read book", MHD_HTTP_OK, connection);
}

int book_update (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  return write_response("Update book", MHD_HTTP_OK, connection);
}

int book_delete (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  return write_response("Delete book", MHD_HTTP_OK, connection);
}

int not_found_handler (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  return write_response("Route not found", MHD_HTTP_NOT_FOUND, connection);
}
