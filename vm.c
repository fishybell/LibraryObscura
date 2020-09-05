#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

struct vm {
  int registers[256];
  int loops[256];
  char *buffers[256];
};

int parse_line(struct vm *v, char *line) {
  int i = 0;
  char key = line[2];
  char key2 = line[4];
  char key3 = line[6];
  //printf("line[0]: %c\n", line[0]);
  //printf("key: %c\n", key);
  //printf("key2: %c\n", key2);
  //printf("key3: %c\n", key3);
  switch (line[0]) {
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

    case 'e': // end a loop (moving up)
      if (++v->loops[key] < key2 - '0') {
        return -1 * (key3 - '0');
      }
      break;

    case 'f': // end a loop (moving down)
      if (--v->loops[key] > key2 - '0') {
        return -1 * (key3 - '0');
      }
      break;

    case 'b': // start a buffer
      v->buffers[key] = malloc(BUFFER_SIZE);
      while (i<BUFFER_SIZE && line[4+i] != 0) {
        v->buffers[key][i] = line[4+i];
        i++;
      }
      
      v->registers[key] = key2;
      break;

    case 'c': // pring a buffer
      printf("%s", v->buffers[key]);
      break;
  }

  return 1; // default of one step forward
}

int main () {
	printf("Should print like so\n");

  for (int i=0; i<3; i++) {
    printf("%c", 'a'+i);
    for (int j=3; j>0; j--)
      printf("%c", '3'+j);
    printf("\n");
  }
  printf("done...\n");

  char *lines[] = {
    "r c a",   // set register c to 'a'
    "l i 0",   // set loop iterator i to 0
    // {
      "a d c i", // add register c to i, store in d
      "p d",     // print register d
      
      "r e 3",   // set register e to '1'
      "l j 3",   // set loop iterator j to 3
      // {
        "a d e j", // add register e to j, store in d
        "p d",     // print register d
        "f j 0 2", // decrement loop iterator j, check against 0, go back 2 steps or move on
        "r d \n",   // set register d to '\n'
      // }

      "p d",     // print register d
      "e i 3 9", // increment loop iterator i, check against 3, go back 2 steps or move on
    // }

    "b a done...\n",  // set buffer a to 'done'
    "c a",         // print buffer a
    NULL
  };

  struct vm v;

  int i = 0;
  while (lines[i] != NULL) {
    //printf("i: %d\n", i);
    i += parse_line(&v, lines[i]);
  }

  return 0;
}
