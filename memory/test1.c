/* CREATED 5-20-14 */

/* Test to show that after allocating and deallocating memory, 
 * trying to deallocate an invalid address is immediately detected.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "slug.h"

#define MAX_ALLOCATION 50

int main (void) {
  void *mem[MAX_ALLOCATION] = { 0 };
  size_t size = 1000;
  int offset = size;
  int i;

  for (i = 0; i < MAX_ALLOCATION; ++i) 
    mem[i] = malloc(size);

  for (i = 0; i < MAX_ALLOCATION; ++i) 
    free(mem[i]);

  /* Trying to free invalid addresses */
  for (i = 0; i < MAX_ALLOCATION; ++i)
    free( mem + i + MAX_ALLOCATION + offset );

  return (EXIT_SUCCESS);
}
