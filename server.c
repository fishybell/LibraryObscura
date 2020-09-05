#include <stdio.h>

#include "types.h"

#define PORT 8080

MHD_AccessHandlerCallback router();

handler_func list_read, book_create, book_read, book_update, book_delete, not_found_handler;
void request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe);

int main () {
  struct MHD_Daemon *daemon;

  MHD_AccessHandlerCallback routes = router(6,
      (struct http_route){
        "GET",
        "^/list$",
        &list_read,
      },
      (struct http_route){
        "POST",
        "^/book/[0-9][0-9Xx-]*$", // not a valid regex for ISBN checking
        &book_create,
      },
      (struct http_route){
        "GET",
        "^/book/[0-9][0-9Xx-]*$", // not a valid regex for ISBN checking
        &book_read,
      },
      (struct http_route){
        "PATCH",
        "^/book/[0-9][0-9Xx-]*$", // not a valid regex for ISBN checking
        &book_update,
      },
      (struct http_route){
        "DELETE",
        "^/book/[0-9][0-9Xx-]*$", // not a valid regex for ISBN checking
        &book_delete,
      },
      (struct http_route){
        "",
        ".*",
        &not_found_handler,
      }
      );

  daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                             routes, NULL,
                             MHD_OPTION_NOTIFY_COMPLETED, request_completed, NULL,
                             MHD_OPTION_END);
  if (NULL == daemon) return 1;

	printf("Listening on port 8080\nPress ENTER to exit\n");
  fflush(stdout);
  getchar ();

  MHD_stop_daemon (daemon);
	printf("Exiting\n");
  return 0;
}
