#include <stdbool.h>
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

struct book {
  char *isbn;
  char *title;
};

struct shelf {
  struct book current;
  struct shelf *next;
};

struct connection_context
{
  char *buffer;
  size_t buffer_size;
  bool too_big;
};
