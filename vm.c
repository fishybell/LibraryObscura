#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

int parse_line(struct vm *v, char *line) {
  printf("processing lines: %s\n", line);
  fflush(stdout);
  int i = 0;
  char key = line[2];
  char key2 = line[4];
  char key3 = line[6];
  void_func func;
  void *param;
  switch (line[0]) {
    case 'u': // point a pointer to a buffer
      param = v->buffers[key2];
      v->pointers[key] = param;
      break;

    case 'x': // exec a function at a pointer
      func = v->pointers[key];
      param = v->pointers + key2;
      func(param);
      break;

    case 'r': // set a register
      v->registers[key] = key2;
      break;

    case 'l': // start a loop
      v->loops[key] = key2 - '0';
      break;

    case 'a': // add a loop to a register
      v->registers[key] = v->registers[key2] + v->loops[key3];
      break;

    case 'p': // print a register
      printf("%c", v->registers[key]);
      break;

    case 'e': // end a loop (incrementing iterator)
      if (++v->loops[key] < key2 - '0') {
        return -1 * (key3 - '0');
      }
      break;

    case 'f': // end a loop (decrementing iterator)
      if (--v->loops[key] > key2 - '0') {
        return -1 * (key3 - '0');
      }
      break;

    case 'm': // match two buffers
      if (strncmp(v->buffers[key], v->buffers[key2], strlen(v->buffers[key])) != 0) {
        i = key3 - '0';
        printf("didn't match buffers 0 and 1, jumping %d\n", i);
        fflush(stdout);
        return i;
      }
      printf("matched buffers 0 and 1, jumping default\n");
      fflush(stdout);
      break;

    case 'o': // get a pointer to a register
      param = malloc(sizeof(int));
      *(int*)param = v->registers[key];
      printf("what will be pointer %c? %d\n", key2, *(int*)param);
      v->pointers[key2] = param;
      break;

    case 'b': // start a buffer
      v->buffers[key] = malloc(BUFFER_SIZE);
      while (i<BUFFER_SIZE && line[4+i] != 0) {
        v->buffers[key][i] = line[4+i];
        i++;
      }

      v->registers[key] = key2;
      break;

    case 'c': // print a buffer
      printf("%s", v->buffers[key]);
      break;
  }

  return 1; // default of one step forward
}

void parse_lines(struct vm *v, char **lines) {
  int i = 0;
  while (lines[i] != NULL) {
    //printf("i: %d\n", i);
    i += parse_line(v, lines[i]);
  }
}

struct vm *new_vm() {
  struct vm *v = malloc(sizeof(struct vm));

  return v;
}

/*void some_func(void *ptr) {
  for (int j=3; j>0; j--)
    printf("%s%c", (char*)ptr, '3'+j);
}

struct vm *new_standard_vm() {
  struct vm *v = malloc(sizeof(struct vm));

  v->pointers['0'] = &some_func;

  return v;
}

int main () {
	printf("Should print like so\n");

  printf("-----------------\n");

  // test block {
  for (int i=0; i<3; i++) {
    printf("%c", 'a'+i);
    for (int j=3; j>0; j--)
      printf("\nHi: %c", '3'+j);
    printf("\n");
  }
  printf("done...\n");
  // }

  printf("-----------------\n");

  char *lines[] = {
    // setup outer loop
    "r c a",                         // set register c to 'a'
    "l i 0",                         // set loop iterator i to 0

    // {
      // print next letter in outer loop
      "a d c i",                     // add register c to i, store in d
      "p d",                         // print register d

      // call external function (inner loop)
      "b a \nHi: ",                  // set buffer a to '\nHi: '
      "u h a",                       // point pointer h to buffer a
      "x 0 h",                       // execute function 0 with pointer h

      // print carraige, return, end outer loop
      "r d \n",                      // set register d to '\n'
      "p d",                         // print register d
      "e i 3 7",                     // increment loop iterator i, check against 3, go back to start of loop
    // }

    // test noop
    "",                              // noop
    "",                              // noop

    // test forward jump
    "l i .",                         // set loop iterator i to -2 (minimum for this trick)
    "e i 0 -",                       // increment loop iterator i, check against 0, jump backwards -3 ('0' - '-' == -3)
    "b a wrong...\n",                // set buffer a to 'wrong...\n'
    "c a",                           // print buffer a

    // finish up
    "b a done...\n",                 // set buffer a to 'done...\n'
    "c a",                           // print buffer a
    NULL
  };

  parse_lines(new_standard_vm(), lines);
  printf("-----------------\n");

  return 0;
}*/
