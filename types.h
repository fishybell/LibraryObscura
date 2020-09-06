#include <stdbool.h>
#include <microhttpd.h>

#define BUFFER_SIZE 1024

#define PTR *(void**)(ptr)
#define MPTR(A, B) ptr += (A - B) * (sizeof(void*))

typedef int (handler_func)(void *, struct MHD_Connection *,
                          const char *,
                          const char *, const char *,
                          const char *,
                          size_t *, void **);

struct http_route {
  char *method;
  char *path;
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

struct vm {
  void *pointers[256];
  int registers[256];
  int loops[256];
  char *buffers[256];
};

typedef void(*void_func)(void*);

