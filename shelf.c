#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

int write_response(char *page, int status, struct MHD_Connection *connection, enum MHD_ResponseMemoryMode mode);

struct connection_info_struct {
  int connectiontype;
  char *answerstring;
  struct MHD_PostProcessor *postprocessor;
};

// a global shelf that starts off empty
static struct shelf global_shelf = {(struct book){NULL, NULL}, NULL};

int list_read (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls) {

  char *result = malloc(sizeof(char) * (BUFFER_SIZE + 20)); // enough extra for our "and many more" and a null termination
	result[0] = 0;
  char item[BUFFER_SIZE];
  int result_size, item_size, handled;
	struct shelf local_shelf = global_shelf;

  // loop through our linked list, building up the response as we go
  result_size = 0;
  while (local_shelf.current.isbn != NULL) {

    item_size = snprintf(item, BUFFER_SIZE, "%s: %s\n", local_shelf.current.isbn, local_shelf.current.title);

    if (result_size + item_size < BUFFER_SIZE) {
      strncat(result, item, item_size);
      result_size += item_size;
    } else {
      strcat(result, "...and many more...\n"); // we left enough room for this
      result_size += 19;
      break;
    }

		if (local_shelf.next == NULL) {
      break;
		}

		local_shelf = *local_shelf.next;
	}

  if (result_size == 0) {
    result_size = sprintf(result, "This is an empty library\n");
  }

  handled = write_response(result, MHD_HTTP_OK, connection, MHD_RESPMEM_MUST_FREE);

  return handled;
}

void add_book_to_shelf (struct book item) {
	struct shelf *shelf_spot = &global_shelf;
	struct shelf *new_item = malloc(sizeof(new_item));

  new_item->current = item;
  new_item->next = NULL;

  // loop through our linked list until we find the last item
  while (shelf_spot->current.isbn != NULL) {

		if (shelf_spot->next == NULL) {
      // last book on shelf
      shelf_spot->next = new_item;
      return;
		}

		shelf_spot = shelf_spot->next;
	}

  // first book on shelf
  *shelf_spot = *new_item;
}

void add_book_to_shelf2 (void *ptr) {
  struct book item;
  item.isbn = (char*)PTR;
  MPTR('c', 'b');
  item.title = (char*)PTR;

  add_book_to_shelf(item);
}

struct book *get_book_from_shelf(char *isbn) {
	struct shelf *shelf_spot = &global_shelf;

  // loop through our linked list until we find our book
  while (shelf_spot->current.isbn != NULL) {

		if (strcmp(shelf_spot->current.isbn, isbn) == 0) {
      // found it
      return &shelf_spot->current;
    } else if (shelf_spot->next == NULL) {
      // not on the shelf
      return NULL;
    }

		shelf_spot = shelf_spot->next;
	}

  // nothing on the shelf
  return NULL;
}

void get_book_from_shelf2 (void *ptr) {
  char *isbn = (char*)PTR;

  struct book *item = get_book_from_shelf (isbn);

  MPTR('d', 'b');
  *(void **)ptr = item == NULL ? "%" : "+";
  printf("I got some stuff from d: %s\n", (char*)PTR);
  if (item) {
    MPTR('b', 'd');
    *(void **)ptr = item->isbn;
    MPTR('c', 'b');
    *(void **)ptr = item->title;
  }
}

bool delete_book_from_shelf(char *isbn) {
	struct shelf *shelf_spot = &global_shelf;
	struct shelf *last_spot = NULL;
	struct shelf *next = NULL;
  struct book current;
  bool top = true;

  // loop through our linked list until we find our book
  while (shelf_spot->current.isbn != NULL) {
    current = shelf_spot->current;
    next = shelf_spot->next;

		if (strcmp(current.isbn, isbn) == 0) {
      // found it
      if (last_spot == NULL) {
        if (next == NULL) {
          // it was the only item on the shelf, clear the shelf
          global_shelf = (struct shelf){(struct book){NULL, NULL}, NULL};
        } else {
          // it was the first item on the shelf, reset our top spot
          global_shelf.current = next->current;
          global_shelf.next = next->next;
        }
      } else {
        // it was a secondary item, remove the reference to it and patch the hole
        last_spot->next = next;
      }

      free(current.isbn);
      free(current.title);
      if (!top) {
        free(shelf_spot);
      }

      return true;
    } else if (next == NULL) {
      // not on the shelf
      return false;
    }

    top = false;
    last_spot = shelf_spot;
		shelf_spot = next;
	}

  // nothing on the shelf
  return false;
}
