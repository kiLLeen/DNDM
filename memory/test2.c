/* CREATED 5-20-14 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "slug.h"

#define MAX_ALLOCATION 50 /* Max allocation in bytes. */

int main() {
    void *mem[MAX_ALLOCATION] = { 0 };
    void *temp[MAX_ALLOCATION] = { 0 };
    int i;

    for (i = 0; i < MAX_ALLOCATION; ++i) {
        mem[i] = malloc(1);
        temp[i] = mem[i];
    }

    for (i = 0; i < MAX_ALLOCATION; ++i) 
        free(mem[i]);

    for (i = 0; i < MAX_ALLOCATION; ++i) /* Trying to free memory that has been freed already. */
        free(temp[i]);

    return 0;
}
