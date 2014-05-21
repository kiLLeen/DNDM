/* CREATED 5-20-14 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "slug.h"

#define MAX_ALLOCATION 200 /* How many allocations */
#define SIZE_OF_ALLOC 10 /* Size of the allocation */

int main(int argc, char *argv[]) {
    void *mem[MAX_ALLOCATION] = { 0 };
    int i;

    for (i = 0; i < MAX_ALLOCATION; ++i) {
        mem[i] = malloc(SIZE_OF_ALLOC);
        printf("Allocating: %p\n", mem[i]);
    }

    printf("Exiting...\n");
    return 0;
}
