#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

int parse_line(struct vm *v, char *line) {
  int i = 0, j = 0;
  char key = line[2];
  char key2 = line[4];
  char key3 = line[6];
  void_func func;
  void *param;
  switch (line[0]) {
    case 'a': // add a loop to a register
      v->registers[key] = v->registers[key2] + v->loops[key3];
      break;

    case 'b': // start a buffer
      v->buffers[key] = malloc(BUFFER_SIZE);
      while (i<BUFFER_SIZE && line[4+i] != 0) {
        v->buffers[key][i] = line[4+i];
        i++;
      }
      v->buffers[key][i] = 0;
      break;

    case 'c': // print a buffer
      printf("%s", v->buffers[key]);
      fflush(stdout);
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

    case 'g': // grow a buffer
      i = strlen(v->buffers[key]);
      j = 0;
      while (i<BUFFER_SIZE && v->buffers[key2][j] != 0) {
        v->buffers[key][i] = v->buffers[key2][j];
        i++;
        j++;
      }
      v->buffers[key][i] = 0;
      break;

    case 'i': // inline edit a buffer
      i = v->loops[key2];
      v->buffers[key][i] = v->registers[key3];
      break;

    case 'l': // start a loop
      v->loops[key] = key2 - '0';
      break;

    case 'm': // match two buffers
      if (strncmp(v->buffers[key], v->buffers[key2], strlen(v->buffers[key])) != 0) {
        i = key3 - '0';
        return i;
      }
      break;

    case 'o': // get a pointer to a register
      param = malloc(sizeof(int));
      *(int*)param = v->registers[key];
      v->pointers[key2] = param;
      break;

    case 'p': // print a register
      printf("%c", v->registers[key]);
      break;

    case 'r': // set a register
      v->registers[key] = key2;
      break;

    case 's': // swap a pointer to a char* with a buffer
      free(*(char**)v->pointers[key]);
      param = v->buffers[key2];
      *(char**)(v->pointers[key]) = param;
      break;

    case 't': // point a buffer to a pointer
      param = v->pointers[key2];
      v->buffers[key] = (char*)param;
      break;

    case 'u': // point a pointer to a buffer
      param = v->buffers[key2];
      v->pointers[key] = param;
      break;

    case 'x': // exec a function at a pointer
      func = v->pointers[key];
      param = v->pointers + key2;
      func(param);
      break;
  }

  return 1; // default of one step forward
}

void parse_lines(struct vm *v, char **lines) {
  int i = 0;
  while (lines[i] != NULL) {
    i += parse_line(v, lines[i]);
  }
}
