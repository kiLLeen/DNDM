/* CREATED 5-20-14 */

/* Test to show errors when trying to free memory with
 * a location in the middle of an allocation.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "slug.h"

#define MAX_ALLOCATION 50 /* How many allocations */
#define SIZE_OF_ALLOC 10 /* Size of the allocation */

int main() {
    void *mem[MAX_ALLOCATION] = { 0 };
    int i;

    for (i = 0; i < MAX_ALLOCATION; ++i) 
        mem[i] = malloc(SIZE_OF_ALLOC);

    /* Trying to free in the middle of the allocation */
    for (i = 0; i < MAX_ALLOCATION; ++i) 
        free(mem[i] + (SIZE_OF_ALLOC / 2));

    /* Actually free the memory */
    for (i = 0; i < MAX_ALLOCATION; ++i)
        free(mem[i]);

    return 0;
}
