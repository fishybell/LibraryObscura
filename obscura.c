#include <stdio.h>
#include <stdbool.h>
#include <microhttpd.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#define PORT 8080
#define S 1024
#define y(A, B) H += (A - B) * (sizeof(void*))
struct h {
  char *M;
  char *P;
  char *L[S];
};
struct h logic[] = { (struct h){"GET","/list",{"xAa","rcd","lib","acci","ej0.","ej0#","acci","oc7","lj1","azzj","oz4","x04","x13","x26",NULL},},(struct h){"POST","/book/",{"rc2","li&","acci","acci","acci","acci","azci","i6jz","ub5","uc6","x9b","b0Created book\n","ua0","rcd","lib","acci","acci","oc7","oz4","x04","x13","x26",NULL},},(struct h){"GET","/book/",{"ub5","xBb","b0Book not found\n","b7%","t8d","rc2","li&","acci","acci","acci","acci","azci","m78>","lj,","rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","ej0 ","lk0","i0kz","t8b","g08","b7:","g07","b7 ","g07","t8c","g08","b7\n","g07","rcd","lib","acci","ej0.","ej0#","acci","oc7","lj1","azzj","oz4","ua0","x04","x13","x26",NULL},},(struct h){"PATCH","/book/",{"ub5","xCb","b0Book not found\n","b7%","t8d","rc2","li&","acci","acci","acci","acci","azci","m78>","lj,","rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","ej0%","sb5","sc6","b0Book updated\n","rcd","lib","acci","acci","oc7","lj1","azzj","oz4","ua0","x04","x13","x26",NULL},},(struct h){"DELETE","/book/",{"ub5","xDb","b0Book not found\n","b7%","t8d","rc2","li&","acci","acci","acci","acci","azci","m78>","lj,","rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","ej0'","b0Book deleted\n","rcd","lib","acci","acci","oc7","lj1","azzj","oz4","ua0","x04","x13","x26",NULL},},(struct h){"","",{"rcd","lib","acci","acci","acci","acci","acci","acci","li4","acci","oc7","oz4","ua9","x04","x13","x26",NULL},},(struct h){"","",{"rc*","lj,","mAB3","mC52","ej0-","li0","ej0.","li1","acci","oc5",NULL}}};
struct D {
  char *F;
  char *I;
};
struct K {
  struct D A;
  struct K *B;
};
struct C {
  char *B;
  size_t Z;
  bool T;
};
struct vm {
  void *P[S];
  int R[S];
  int L[S];
  char *U[S];
};
typedef void(*f)(void*);
int p(struct vm *v, char *l) {
  int i = 0, j = 0;
  void *H;
  switch (l[0]) {
    case 'a':
      v->R[l[1]] = v->R[l[2]] + v->L[l[3]];
      break;
    case 'b':
      v->U[l[1]] = malloc(S);
      while (i<S && l[2+i] != 0) {
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
      while (i<S && v->U[l[2]][j] != 0) {
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
      H = malloc(sizeof(int));
      *(int*)H = v->R[l[1]];
      v->P[l[2]] = H;
      break;
    case 'r':
      v->R[l[1]] = l[2];
      break;
    case 's':
      free(*(char**)v->P[l[1]]);
      H = v->U[l[2]];
      *(char**)(v->P[l[1]]) = H;
      break;
    case 't':
      H = v->P[l[2]];
      v->U[l[1]] = (char*)H;
      break;
    case 'u':
      H = v->U[l[2]];
      v->P[l[1]] = H;
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
static struct K library = {(struct D){NULL, NULL}, NULL};
void e (void *H) {
  char *N = malloc(S);
	N[0] = 0;
  char J[S];
  int O, G;
	struct K Q = library;
  O = 0;
  while (Q.A.F != NULL) {
    G = snprintf(J, S, "%s: %s\n", Q.A.F, Q.A.I);
    if (O + G < S - 20) {
      strncat(N, J, G);
      O += G;
    } else {
      strcat(N, "...and many more...\n");
      O += 19;
      break;
    }
		if (Q.B == NULL) {
      break;
		}
		Q = *Q.B;
	}
  if (O == 0) {
    sprintf(N, "This is an empty library\n");
  }
  *(void **)H = N;
}
void o (void *H) {
  struct D J;
  J.F = (char*)*(void**)(H);
  y('c', 'b');
  J.I = (char*)*(void**)(H);
	struct K *w = &library;
	struct K *O = malloc(sizeof(O));
  O->A = J;
  O->B = NULL;
  while (w->A.F != NULL) {
		if (w->B == NULL) {
      w->B = O;
      return;
		}
		w = w->B;
	}
  *w = *O;
}
void g (void *H) {
  char *F = (char*)*(void**)(H);
  y('d', 'b');
  *(void **)H = "%";
  struct D *J = NULL;
	struct K *w = &library;
  while (w->A.F != NULL) {
		if (strcmp(w->A.F, F) == 0) {
      J = &w->A;
      *(void **)H = "+";
      y('b', 'd');
      *(void **)H = J->F;
      y('c', 'b');
      *(void **)H = J->I;
      break;
    } else if (w->B == NULL) {
      break;
    }
		w = w->B;
	}
}
void d (void *H) {
  char *F = (char*)*(void**)(H);
  y('d', 'b');
  *(void **)H = "%";
  struct D *J = NULL;
	struct K *w = &library;
  while (w->A.F != NULL) {
		if (strcmp(w->A.F, F) == 0) {
      J = &w->A;
      *(void **)H = "+";
      y('b', 'd');
      *(void **)H = &J->F;
      y('c', 'b');
      *(void **)H = &J->I;
      break;
    } else if (w->B == NULL) {
      break;
    }
		w = w->B;
	}
}
void c (void *H) {
  char *F = (char*)*(void**)(H);
  y('d', 'b');
  *(void **)H = "%";
	struct K *w = &library;
	struct K *s = NULL;
	struct K *B = NULL;
  struct D A;
  bool O = true;
  while (w->A.F != NULL) {
    A = w->A;
    B = w->B;
		if (strcmp(A.F, F) == 0) {
      if (s == NULL) {
        if (B == NULL) {
          library = (struct K){(struct D){NULL, NULL}, NULL};
        } else {
          library.A = B->A;
          library.B = B->B;
        }
      } else {
        s->B = B;
      }
      free(A.F);
      free(A.I);
      if (!O) {
        free(w);
      }
      *(void **)H = "+";
      return;
    } else if (B == NULL) {
      return;
    }
    O = false;
    s = w;
		w = B;
	}
}
void r(void *H) {
  int mode = *(int*)*(void**)(H);
  y('a', '4');
  char *page = (char*)*(void**)(H);
  y('6', 'a');
  *(void **)H = MHD_create_response_from_buffer(strlen(page), (void*)page, mode);
}
void x(void *H) {
  struct MHD_Connection *n = (struct MHD_Connection *)*(void**)(H);
  y('6', '3');
  struct MHD_Response *O = (struct MHD_Response *)*(void**)(H);
  y('7', '6');
  int status = *(int*)*(void**)(H);
  y('5', '7');
  *(int*)(void **)H = MHD_queue_response (n, status, O);
}
void a(void *H) {
  struct MHD_Response *O = (struct MHD_Response *)*(void**)(H);
  MHD_destroy_response (O);
}
void
k (void *cls, struct MHD_Connection *n,
                   void **U, enum MHD_RequestTerminationCode O) {
  struct C *t = *U;
  if (t == NULL)
    return;
  free (t->B);
  free (t);
  *U = NULL;
}
int q (void *cls, struct MHD_Connection *n, \
                          const char *url, \
                          const char *M, const char *O, \
                          const char *Y, \
                          size_t *z, void **U){
  struct C *t;
  int i, ret;
  struct vm *v = malloc(sizeof(struct vm));
  char *u, *b, *m;
  if (*U == NULL) {
    t = malloc (sizeof (struct C));
    if (t == NULL) {
      return MHD_NO;
    }
    t->B = malloc(S);
    if (t->B == NULL) {
      return MHD_NO;
    }
    t->Z = 0;
    t->T = false;
    *U = (void *) t;
    return MHD_YES;
  }
  t = *U;
  if (z != NULL) {
    if (*z != 0) {
      if (t->Z + *z < S) {
        memcpy(t->B + t->Z, Y, *z);
        t->Z += *z;
      } else {
        t->T = true;
      }
      *z = 0;
      return MHD_YES;
    }
  }
  i = 0;
  u = malloc(S);
  b = malloc(S);
  m = malloc(S);
  strncpy(u, url+6, S);
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
                             &q, NULL,
                             MHD_OPTION_NOTIFY_COMPLETED, k, NULL,
                             MHD_OPTION_END);
  if (NULL == m) return 1;
  fflush(stdout);
  getchar ();
  MHD_stop_daemon (m);
  return 0;
}
