#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

#define PORT 8080

MHD_AccessHandlerCallback router();

handler_func handler1, handler2, not_found_handler;

int main () {
  struct MHD_Daemon *daemon;

  MHD_AccessHandlerCallback routes = router(3,
      (struct http_route){
        "GET",
        ".*",
        &handler1,
      },
      (struct http_route){
        "POST",
        ".*",
        &handler2,
      },
      (struct http_route){
        "",
        ".*",
        &not_found_handler,
      }
      );

  daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                             routes, NULL, MHD_OPTION_END);
  if (NULL == daemon) return 1;

	printf("Listening on port 8080\nPress ENTER to exit\n");
  fflush(stdout);
  getchar ();

  MHD_stop_daemon (daemon);
	printf("Exiting\n");
  return 0;
}
