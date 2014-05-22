/* CREATED 5-20-14 */

/* Test to show that after allocating and deallocating memory, 
 * trying to deallocate an invalid address is immediately detected.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "slug.h"

#define MAX_ALLOCATION 50
#define SIZE_OF_ALLOC 1000

int main() {
  void *mem[MAX_ALLOCATION] = { 0 };
  int offset = SIZE_OF_ALLOC;
  int i;

  for (i = 0; i < MAX_ALLOCATION; ++i) 
    mem[i] = malloc(SIZE_OF_ALLOC);

  for (i = 0; i < MAX_ALLOCATION; ++i) 
    free(mem[i]);

  /* Trying to free invalid addresses */
  for (i = 0; i < MAX_ALLOCATION; ++i)
    free( mem + i + MAX_ALLOCATION + offset );

  return 0;
}
