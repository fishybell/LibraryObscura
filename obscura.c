#include <stdio.h>
#include <stdbool.h>
#include <microhttpd.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#define PORT 8080
#define BUFFER_SIZE 1024
#define PTR *(void**)(ptr)
#define MPTR(A, B) ptr += (A - B) * (sizeof(void*))
#define HANDLER (void *cls, struct MHD_Connection *connection, \
                          const char *url, \
                          const char *method, const char *version, \
                          const char *upload_data, \
                          size_t *upload_data_size, void **con_cls)
struct http_route {
  char *method;
  char *path;
  char *lines[BUFFER_SIZE]; 
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
int parse_line(struct vm *v, char *line) {
  int i = 0, j = 0;
  char key = line[1];
  char key2 = line[2];
  char key3 = line[3];
  void_func func;
  void *param;
  switch (line[0]) {
    case 'a':
      v->registers[key] = v->registers[key2] + v->loops[key3];
      break;
    case 'b':
      v->buffers[key] = malloc(BUFFER_SIZE);
      while (i<BUFFER_SIZE && line[2+i] != 0) {
        v->buffers[key][i] = line[2+i];
        i++;
      }
      v->buffers[key][i] = 0;
      break;
    case 'c':
      printf("%s", v->buffers[key]);
      fflush(stdout);
      break;
    case 'e':
      if (++v->loops[key] < key2 - '0') {
        return -1 * (key3 - '0');
      }
      break;
    case 'f':
      if (--v->loops[key] > key2 - '0') {
        return -1 * (key3 - '0');
      }
      break;
    case 'g':
      i = strlen(v->buffers[key]);
      j = 0;
      while (i<BUFFER_SIZE && v->buffers[key2][j] != 0) {
        v->buffers[key][i] = v->buffers[key2][j];
        i++;
        j++;
      }
      v->buffers[key][i] = 0;
      break;
    case 'i':
      i = v->loops[key2];
      v->buffers[key][i] = v->registers[key3];
      break;
    case 'l':
      v->loops[key] = key2 - '0';
      break;
    case 'm':
      if (strncmp(v->buffers[key], v->buffers[key2], strlen(v->buffers[key])) != 0) {
        i = key3 - '0';
        return i;
      }
      break;
    case 'o':
      param = malloc(sizeof(int));
      *(int*)param = v->registers[key];
      v->pointers[key2] = param;
      break;
    case 'p':
      printf("%c", v->registers[key]);
      break;
    case 'r':
      v->registers[key] = key2;
      break;
    case 's':
      free(*(char**)v->pointers[key]);
      param = v->buffers[key2];
      *(char**)(v->pointers[key]) = param;
      break;
    case 't':
      param = v->pointers[key2];
      v->buffers[key] = (char*)param;
      break;
    case 'u':
      param = v->buffers[key2];
      v->pointers[key] = param;
      break;
    case 'x':
      func = v->pointers[key];
      param = v->pointers + key2;
      func(param);
      break;
  }
  return 1;
}
void parse_lines(struct vm *v, char **lines) {
  int i = 0;
  while (lines[i] != NULL) {
    i += parse_line(v, lines[i]);
  }
}
struct connection_info_struct {
  int connectiontype;
  char *answerstring;
  struct MHD_PostProcessor *postprocessor;
};
static struct shelf global_shelf = {(struct book){NULL, NULL}, NULL};
void list_read2 (void *ptr) {
  char *result = malloc(BUFFER_SIZE);
	result[0] = 0;
  char item[BUFFER_SIZE];
  int result_size, item_size, handled;
	struct shelf local_shelf = global_shelf;
  result_size = 0;
  while (local_shelf.current.isbn != NULL) {
    item_size = snprintf(item, BUFFER_SIZE, "%s: %s\n", local_shelf.current.isbn, local_shelf.current.title);
    if (result_size + item_size < BUFFER_SIZE - 20) {
      strncat(result, item, item_size);
      result_size += item_size;
    } else {
      strcat(result, "...and many more...\n");
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
  while (shelf_spot->current.isbn != NULL) {
		if (shelf_spot->next == NULL) {
      shelf_spot->next = new_item;
      return;
		}
		shelf_spot = shelf_spot->next;
	}
  *shelf_spot = *new_item;
}
void get_book_from_shelf2 (void *ptr) {
  char *isbn = (char*)PTR;
  MPTR('d', 'b');
  *(void **)ptr = "%";
  struct book *item = NULL;
	struct shelf *shelf_spot = &global_shelf;
  while (shelf_spot->current.isbn != NULL) {
		if (strcmp(shelf_spot->current.isbn, isbn) == 0) {
      item = &shelf_spot->current;
      *(void **)ptr = "+";
      MPTR('b', 'd');
      *(void **)ptr = item->isbn;
      MPTR('c', 'b');
      *(void **)ptr = item->title;
      break;
    } else if (shelf_spot->next == NULL) {
      break;
    }
		shelf_spot = shelf_spot->next;
	}
}
void get_book_from_shelf3 (void *ptr) {
  char *isbn = (char*)PTR;
  MPTR('d', 'b');
  *(void **)ptr = "%";
  struct book *item = NULL;
	struct shelf *shelf_spot = &global_shelf;
  while (shelf_spot->current.isbn != NULL) {
		if (strcmp(shelf_spot->current.isbn, isbn) == 0) {
      item = &shelf_spot->current;
      *(void **)ptr = "+";
      MPTR('b', 'd');
      *(void **)ptr = &item->isbn;
      MPTR('c', 'b');
      *(void **)ptr = &item->title;
      break;
    } else if (shelf_spot->next == NULL) {
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
  while (shelf_spot->current.isbn != NULL) {
    current = shelf_spot->current;
    next = shelf_spot->next;
		if (strcmp(current.isbn, isbn) == 0) {
      if (last_spot == NULL) {
        if (next == NULL) {
          global_shelf = (struct shelf){(struct book){NULL, NULL}, NULL};
        } else {
          global_shelf.current = next->current;
          global_shelf.next = next->next;
        }
      } else {
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
      return;
    }
    top = false;
    last_spot = shelf_spot;
		shelf_spot = next;
	}
}
void create_response_from_buffer(void *ptr) {
  int mode = *(int*)PTR;
  MPTR('a', '4');
  char *page = (char*)PTR;
  MPTR('6', 'a');
  *(void **)ptr = MHD_create_response_from_buffer(strlen(page), (void*)page, mode);
}
void queue_response(void *ptr) {
  struct MHD_Connection *connection = (struct MHD_Connection *)PTR;
  MPTR('6', '3');
  struct MHD_Response *response = (struct MHD_Response *)PTR;
  MPTR('7', '6');
  int status = *(int*)PTR;
  MPTR('5', '7');
  *(int*)(void **)ptr = MHD_queue_response (connection, status, response);
}
void destroy_response(void *ptr) {
  struct MHD_Response *response = (struct MHD_Response *)PTR;
  MHD_destroy_response (response);
}
void
request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe) {
  struct connection_context *context = *con_cls;
  if (context == NULL)
    return;
  free (context->buffer);
  free (context);
  *con_cls = NULL;
}
int route_to_handler HANDLER {
  struct connection_context *context;
  int i, ret;
  struct vm *v = malloc(sizeof(struct vm));
  char *u, *b, *m;
  static struct http_route logic[] = { (struct http_route){"GET","/list",{"xAa","rcd","lib","acci","ej0.","ej0#","acci","oc7","lj1","azzj","oz4","x04","x13","x26",NULL},},(struct http_route){"POST","/book/",{"rc2","li&","acci","acci","acci","acci","azci","i6jz","ub5","uc6","x9b","b0Created book\n","ua0","rcd","lib","acci","acci","oc7","oz4","x04","x13","x26",NULL},},(struct http_route){"GET","/book/",{"ub5","xBb","b0Book not found\n","b7%","t8d","rc2","li&","acci","acci","acci","acci","azci","m78>","lj,","rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","ej0 ","lk0","i0kz","t8b","g08","b7:","g07","b7 ","g07","t8c","g08","b7\n","g07","rcd","lib","acci","ej0.","ej0#","acci","oc7","lj1","azzj","oz4","ua0","x04","x13","x26",NULL},},(struct http_route){"PATCH","/book/",{"ub5","xCb","b0Book not found\n","b7%","t8d","rc2","li&","acci","acci","acci","acci","azci","m78>","lj,","rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","ej0%","sb5","sc6","b0Book updated\n","rcd","lib","acci","acci","oc7","lj1","azzj","oz4","ua0","x04","x13","x26",NULL},},(struct http_route){"DELETE","/book/",{"ub5","xDb","b0Book not found\n","b7%","t8d","rc2","li&","acci","acci","acci","acci","azci","m78>","lj,","rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","ej0'","b0Book deleted\n","rcd","lib","acci","acci","oc7","lj1","azzj","oz4","ua0","x04","x13","x26",NULL},},(struct http_route){"","",{"rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","oz4","ua9","x04","x13","x26",NULL},},(struct http_route){"","",{"rc*","lj,","mAB3","mC52","ej0-","li0","ej0.","li1","acci","oc5",NULL}}};
  if (*con_cls == NULL) {
    context = malloc (sizeof (struct connection_context));
    if (context == NULL) {
      return MHD_NO;
    }
    context->buffer = malloc(BUFFER_SIZE);
    if (context->buffer == NULL) {
      return MHD_NO;
    }
    context->buffer_size = 0;
    context->too_big = false;
    *con_cls = (void *) context;
    return MHD_YES;
  }
  context = *con_cls;
  printf("url: %s\n", url);
  printf("method: %s\n", method);
  printf("bytes: %ld\n", context->buffer_size);
  if (upload_data_size != NULL) {
    if (*upload_data_size != 0) {
      if (context->buffer_size + *upload_data_size < BUFFER_SIZE) {
        memcpy(context->buffer + context->buffer_size, upload_data, *upload_data_size);
        context->buffer_size += *upload_data_size;
      } else {
        context->too_big = true;
      }
      *upload_data_size = 0;
      return MHD_YES;
    }
  }
  i = 0;
  u = malloc(BUFFER_SIZE);
  b = malloc(BUFFER_SIZE);
  m = malloc(BUFFER_SIZE);
  strncpy(u, url+6, BUFFER_SIZE);
  strcpy(b, context->buffer);
  strcpy(u, url);
  strcpy(m, method);
  v->buffers['5'] = u;
  v->buffers['6'] = b;
  v->buffers['9'] = "Route not found\n";
  v->buffers['B'] = m;
  v->pointers['0'] = &create_response_from_buffer;
  v->pointers['1'] = &queue_response;
  v->pointers['2'] = &destroy_response;
  v->pointers['3'] = connection;
  v->pointers['9'] = &add_book_to_shelf2;
  v->pointers['A'] = &list_read2;
  v->pointers['B'] = &get_book_from_shelf2;
  v->pointers['C'] = &get_book_from_shelf3;
  v->pointers['D'] = &delete_book_from_shelf2;
  v->loops['j'] = context->buffer_size+1;
  if (context->too_big) {
    v->buffers['9'] = "Title too long\n";
    parse_lines(v, logic[5].lines);
    ret = *(int*)(void **)(v->pointers + '5');
    v->buffers['9'] = "Route not found\n";
    return ret;
  }
  for(;;i++) {
    v->buffers['A'] = logic[i].method;
    v->buffers['C'] = logic[i].path;
    parse_lines(v, logic[6].lines);
    ret = *(int*)(void **)(v->pointers['5']);
    if (ret - 42) {
      parse_lines(v, logic[i].lines);
      return *(int*)(void **)(v->pointers + '5');
    }
  }
  fprintf(stderr, "No route defined, disconnecting\n");
  return MHD_NO;
}
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
