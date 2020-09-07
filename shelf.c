#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

struct connection_info_struct {
  int connectiontype;
  char *answerstring;
  struct MHD_PostProcessor *postprocessor;
};

// a global shelf that starts off empty
static struct shelf global_shelf = {(struct book){NULL, NULL}, NULL};

void list_read2 (void *ptr) {
  char *result = malloc(BUFFER_SIZE); // enough extra for our "and many more" and a null termination
	result[0] = 0;
  char item[BUFFER_SIZE];
  int result_size, item_size, handled;
	struct shelf local_shelf = global_shelf;

  // loop through our linked list, building up the response as we go
  result_size = 0;
  while (local_shelf.current.isbn != NULL) {

    item_size = snprintf(item, BUFFER_SIZE, "%s: %s\n", local_shelf.current.isbn, local_shelf.current.title);

    if (result_size + item_size < BUFFER_SIZE - 20) {
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
    sprintf(result, "This is an empty library\n");
  }

  *(void **)ptr = result;
}

void add_book_to_shelf2 (void *ptr) {
  struct book item;
  item.isbn = (char*)PTR;
  MPTR('c', 'b');
  item.title = (char*)PTR;

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

void get_book_from_shelf2 (void *ptr) {
  char *isbn = (char*)PTR;
  MPTR('d', 'b');
  *(void **)ptr = "%";

  struct book *item = NULL;
	struct shelf *shelf_spot = &global_shelf;

  // loop through our linked list until we find our book
  while (shelf_spot->current.isbn != NULL) {

		if (strcmp(shelf_spot->current.isbn, isbn) == 0) {
      // found it
      item = &shelf_spot->current;
      *(void **)ptr = "+";
      MPTR('b', 'd');
      *(void **)ptr = item->isbn;
      MPTR('c', 'b');
      *(void **)ptr = item->title;
      break;
    } else if (shelf_spot->next == NULL) {
      // not on the shelf
      break;
    }

		shelf_spot = shelf_spot->next;
	}
}

// gets pointers to the book, not the book itself
void get_book_from_shelf3 (void *ptr) {
  char *isbn = (char*)PTR;
  MPTR('d', 'b');
  *(void **)ptr = "%";

  struct book *item = NULL;
	struct shelf *shelf_spot = &global_shelf;

  // loop through our linked list until we find our book
  while (shelf_spot->current.isbn != NULL) {

		if (strcmp(shelf_spot->current.isbn, isbn) == 0) {
      // found it
      item = &shelf_spot->current;
      *(void **)ptr = "+";
      MPTR('b', 'd');
      *(void **)ptr = &item->isbn;
      MPTR('c', 'b');
      *(void **)ptr = &item->title;
      break;
    } else if (shelf_spot->next == NULL) {
      // not on the shelf
      break;
    }

		shelf_spot = shelf_spot->next;
	}
}

void delete_book_from_shelf2 (void *ptr) {
  char *isbn = (char*)PTR;
  MPTR('d', 'b');
  *(void **)ptr = "%";

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

      *(void **)ptr = "+";
      return;
    } else if (next == NULL) {
      // not on the shelf
      return;
    }

    top = false;
    last_spot = shelf_spot;
		shelf_spot = next;
	}
}
