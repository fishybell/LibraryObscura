#include <stdio.h>

#include "types.h"

#define PORT 8080

int route_to_handler HANDLER;

handler_func list_read, book_create, book_read, book_update, book_delete, not_found_handler;
void request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe);

int main () {
  struct MHD_Daemon *daemon;

  daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                             &route_to_handler, NULL,
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
