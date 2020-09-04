#include <microhttpd.h>

typedef int (handler_func)(void *, struct MHD_Connection *,
                          const char *,
                          const char *, const char *,
                          const char *,
                          size_t *, void **);

struct http_route {
  char *method;
  char *path_regex;
  handler_func *func;
};
