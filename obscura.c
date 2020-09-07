#include <stdio.h>
#include <stdbool.h>
#include <microhttpd.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#define PORT 8080
#define BUFFER_SIZE 1024
#define y(A, B) ptr += (A - B) * (sizeof(void*))
struct book {
  char *isbn;
  char *title;
};
struct shelf {
  struct book current;
  struct shelf *next;
};

struct h {
  char *M;
  char *P;
  char *L[BUFFER_SIZE]; 
};
struct C {
  char *B;
  size_t Z;
  bool T;
};
struct vm {
  void *P[256];
  int R[256];
  int L[256];
  char *U[256];
};
typedef void(*f)(void*);
int p(struct vm *v, char *l) {
  int i = 0, j = 0;
  void *ptr;
  switch (l[0]) {
    case 'a':
      v->R[l[1]] = v->R[l[2]] + v->L[l[3]];
      break;
    case 'b':
      v->U[l[1]] = malloc(BUFFER_SIZE);
      while (i<BUFFER_SIZE && l[2+i] != 0) {
        v->U[l[1]][i] = l[2+i];
        i++;
      }
      v->U[l[1]][i] = 0;
      break;
    case 'e':
      if (++v->L[l[1]] < l[2] - '0') {
        return -1 * (l[3] - '0');
      }
      break;
    case 'g':
      i = strlen(v->U[l[1]]);
      j = 0;
      while (i<BUFFER_SIZE && v->U[l[2]][j] != 0) {
        v->U[l[1]][i] = v->U[l[2]][j];
        i++;
        j++;
      }
      v->U[l[1]][i] = 0;
      break;
    case 'i':
      i = v->L[l[2]];
      v->U[l[1]][i] = v->R[l[3]];
      break;
    case 'l':
      v->L[l[1]] = l[2] - '0';
      break;
    case 'm':
      if (strncmp(v->U[l[1]], v->U[l[2]], strlen(v->U[l[1]])) != 0) {
        i = l[3] - '0';
        return i;
      }
      break;
    case 'o':
      ptr = malloc(sizeof(int));
      *(int*)ptr = v->R[l[1]];
      v->P[l[2]] = ptr;
      break;
    case 'r':
      v->R[l[1]] = l[2];
      break;
    case 's':
      free(*(char**)v->P[l[1]]);
      ptr = v->U[l[2]];
      *(char**)(v->P[l[1]]) = ptr;
      break;
    case 't':
      ptr = v->P[l[2]];
      v->U[l[1]] = (char*)ptr;
      break;
    case 'u':
      ptr = v->U[l[2]];
      v->P[l[1]] = ptr;
      break;
    case 'x':
      ((f)(v->P[l[1]]))(v->P + l[2]);
      break;
  }
  return 1;
}
void ps(struct vm *v, char **L) {
  int i = 0;
  while (L[i] != NULL) {
    i += p(v, L[i]);
  }
}
struct connection_info_struct {
  int connectiontype;
  char *answerstring;
  struct MHD_PostProcessor *postprocessor;
};
static struct shelf global_shelf = {(struct book){NULL, NULL}, NULL};
void e (void *ptr) {
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
void o (void *ptr) {
  struct book item;
  item.isbn = (char*)*(void**)(ptr);
  y('c', 'b');
  item.title = (char*)*(void**)(ptr);
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
void g (void *ptr) {
  char *isbn = (char*)*(void**)(ptr);
  y('d', 'b');
  *(void **)ptr = "%";
  struct book *item = NULL;
	struct shelf *shelf_spot = &global_shelf;
  while (shelf_spot->current.isbn != NULL) {
		if (strcmp(shelf_spot->current.isbn, isbn) == 0) {
      item = &shelf_spot->current;
      *(void **)ptr = "+";
      y('b', 'd');
      *(void **)ptr = item->isbn;
      y('c', 'b');
      *(void **)ptr = item->title;
      break;
    } else if (shelf_spot->next == NULL) {
      break;
    }
		shelf_spot = shelf_spot->next;
	}
}
void d (void *ptr) {
  char *isbn = (char*)*(void**)(ptr);
  y('d', 'b');
  *(void **)ptr = "%";
  struct book *item = NULL;
	struct shelf *shelf_spot = &global_shelf;
  while (shelf_spot->current.isbn != NULL) {
		if (strcmp(shelf_spot->current.isbn, isbn) == 0) {
      item = &shelf_spot->current;
      *(void **)ptr = "+";
      y('b', 'd');
      *(void **)ptr = &item->isbn;
      y('c', 'b');
      *(void **)ptr = &item->title;
      break;
    } else if (shelf_spot->next == NULL) {
      break;
    }
		shelf_spot = shelf_spot->next;
	}
}
void c (void *ptr) {
  char *isbn = (char*)*(void**)(ptr);
  y('d', 'b');
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
void r(void *ptr) {
  int mode = *(int*)*(void**)(ptr);
  y('a', '4');
  char *page = (char*)*(void**)(ptr);
  y('6', 'a');
  *(void **)ptr = MHD_create_response_from_buffer(strlen(page), (void*)page, mode);
}
void x(void *ptr) {
  struct MHD_Connection *n = (struct MHD_Connection *)*(void**)(ptr);
  y('6', '3');
  struct MHD_Response *response = (struct MHD_Response *)*(void**)(ptr);
  y('7', '6');
  int status = *(int*)*(void**)(ptr);
  y('5', '7');
  *(int*)(void **)ptr = MHD_queue_response (n, status, response);
}
void a(void *ptr) {
  struct MHD_Response *response = (struct MHD_Response *)*(void**)(ptr);
  MHD_destroy_response (response);
}
void
request_completed (void *cls, struct MHD_Connection *n,
                   void **con_cls, enum MHD_RequestTerminationCode toe) {
  struct C *t = *con_cls;
  if (t == NULL)
    return;
  free (t->B);
  free (t);
  *con_cls = NULL;
}
int route_to_handler (void *cls, struct MHD_Connection *n, \
                          const char *url, \
                          const char *M, const char *version, \
                          const char *upload_data, \
                          size_t *upload_data_size, void **con_cls){
  struct C *t;
  int i, ret;
  struct vm *v = malloc(sizeof(struct vm));
  char *u, *b, *m;
  static struct h logic[] = { (struct h){"GET","/list",{"xAa","rcd","lib","acci","ej0.","ej0#","acci","oc7","lj1","azzj","oz4","x04","x13","x26",NULL},},(struct h){"POST","/book/",{"rc2","li&","acci","acci","acci","acci","azci","i6jz","ub5","uc6","x9b","b0Created book\n","ua0","rcd","lib","acci","acci","oc7","oz4","x04","x13","x26",NULL},},(struct h){"GET","/book/",{"ub5","xBb","b0Book not found\n","b7%","t8d","rc2","li&","acci","acci","acci","acci","azci","m78>","lj,","rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","ej0 ","lk0","i0kz","t8b","g08","b7:","g07","b7 ","g07","t8c","g08","b7\n","g07","rcd","lib","acci","ej0.","ej0#","acci","oc7","lj1","azzj","oz4","ua0","x04","x13","x26",NULL},},(struct h){"PATCH","/book/",{"ub5","xCb","b0Book not found\n","b7%","t8d","rc2","li&","acci","acci","acci","acci","azci","m78>","lj,","rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","ej0%","sb5","sc6","b0Book updated\n","rcd","lib","acci","acci","oc7","lj1","azzj","oz4","ua0","x04","x13","x26",NULL},},(struct h){"DELETE","/book/",{"ub5","xDb","b0Book not found\n","b7%","t8d","rc2","li&","acci","acci","acci","acci","azci","m78>","lj,","rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","ej0'","b0Book deleted\n","rcd","lib","acci","acci","oc7","lj1","azzj","oz4","ua0","x04","x13","x26",NULL},},(struct h){"","",{"rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","oz4","ua9","x04","x13","x26",NULL},},(struct h){"","",{"rc*","lj,","mAB3","mC52","ej0-","li0","ej0.","li1","acci","oc5",NULL}}};
  if (*con_cls == NULL) {
    t = malloc (sizeof (struct C));
    if (t == NULL) {
      return MHD_NO;
    }
    t->B = malloc(BUFFER_SIZE);
    if (t->B == NULL) {
      return MHD_NO;
    }
    t->Z = 0;
    t->T = false;
    *con_cls = (void *) t;
    return MHD_YES;
  }
  t = *con_cls;
  if (upload_data_size != NULL) {
    if (*upload_data_size != 0) {
      if (t->Z + *upload_data_size < BUFFER_SIZE) {
        memcpy(t->B + t->Z, upload_data, *upload_data_size);
        t->Z += *upload_data_size;
      } else {
        t->T = true;
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
  strcpy(b, t->B);
  strcpy(u, url);
  strcpy(m, M);
  v->U['5'] = u;
  v->U['6'] = b;
  v->U['9'] = "Route not found\n";
  v->U['B'] = m;
  v->P['0'] = &r;
  v->P['1'] = &x;
  v->P['2'] = &a;
  v->P['3'] = n;
  v->P['9'] = &o;
  v->P['A'] = &e;
  v->P['B'] = &g;
  v->P['C'] = &d;
  v->P['D'] = &c;
  v->L['j'] = t->Z+1;
  if (t->T) {
    v->U['9'] = "Title too long\n";
    ps(v, logic[5].L);
    ret = *(int*)(void **)(v->P + '5');
    v->U['9'] = "Route not found\n";
    return ret;
  }
  for(;;i++) {
    v->U['A'] = logic[i].M;
    v->U['C'] = logic[i].P;
    ps(v, logic[6].L);
    ret = *(int*)(void **)(v->P['5']);
    if (ret - 42) {
      ps(v, logic[i].L);
      return *(int*)(void **)(v->P + '5');
    }
  }
  return MHD_NO;
}
int main () {
  struct MHD_Daemon *m;
  m = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                             &route_to_handler, NULL,
                             MHD_OPTION_NOTIFY_COMPLETED, request_completed, NULL,
                             MHD_OPTION_END);
  if (NULL == m) return 1;
  fflush(stdout);
  getchar ();
  MHD_stop_daemon (m);
  return 0;
}
