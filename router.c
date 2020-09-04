#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <stdbool.h>

#include "types.h"

#define BUFFER_SIZE 512

static struct http_route *global_routes;
static struct http_route *global_routes;

bool route_matches(struct http_route route, const char *url, const char *method) {
  int t;
  regex_t re;
  char    buffer[BUFFER_SIZE];

  if ((t=regcomp( &re, route.path_regex, REG_NOSUB )) != 0) {
    regerror(t, &re, buffer, sizeof buffer);
    fprintf(stderr,"Invalid route regex: %s (%s)\n", buffer, route.path_regex);

    exit(1);
  }

  if (strncmp(route.method, method, strlen(route.method)) == 0 &&
      regexec( &re, url, 0, NULL, 0 ) == 0) {
    return true;
  }

  return false;
}

int route_to_handler (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {
  int i;

  printf("url: %s\n", url);
  printf("method: %s\n", method);

  i = 0;
  while(global_routes[i].func != NULL) {
    if (route_matches(global_routes[i], url, method)) {
      return global_routes[i].func(cls, connection, url, method, version, upload_data, upload_data_size, con_cls);
    }

    i++;
  }

  fprintf(stderr, "No route defined, disconnecting\n");

  return MHD_NO;
}

MHD_AccessHandlerCallback router(int count, ...) {
  // Closures don't really exist in C. The standard methodolgy is to use a nested callback function
  // Unfortunately that enables an executable stack, which is discouraged
  // Instead we're breaking consistency by using a singleton object that's shared between 2 functions in this file.
  // Each time this function is called it overwrites the previous routing.

  va_list args;
  int i, sum;

  global_routes=malloc(sizeof(handler_func) * (count)+1);

  va_start (args, count);         /* Initialize the argument list. */

  for (i = 0; i < count; i++) {
    global_routes[i]  = va_arg (args, struct http_route);    /* Get the next argument value. */
  }
  global_routes[i] = (struct http_route){NULL, NULL, NULL};

  va_end (args);                  /* Clean up. */

  return &route_to_handler;
}
