#include <string.h>

#include "types.h"

int write_response(char *page, int status, struct MHD_Connection *connection, enum MHD_ResponseMemoryMode mode);

int book_create (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  return write_response("Create book", MHD_HTTP_OK, connection, MHD_RESPMEM_PERSISTENT);
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
