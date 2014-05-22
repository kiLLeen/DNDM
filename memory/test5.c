/* CREATED 5-20-14 */

/* Test program to show that a correct program remains correct
 * and allocates/deallocates memory as normal, without errors.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "slug.h"

#define MAX_ALLOCATION 200 /* How many allocations */
#define SIZE_OF_ALLOC 10 /* Size of the allocation */

int main() {
    void *mem[MAX_ALLOCATION] = { 0 };
    int i;

    for (i = 0; i < MAX_ALLOCATION; ++i)
        mem[i] = malloc(SIZE_OF_ALLOC);

    for (i = 0; i < MAX_ALLOCATION; ++i)
        free(mem[i]);

    return 0;
}
