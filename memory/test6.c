/* CREATED 5-20-14 */

/* Test program to show that a program attempting to
   allocate 0 bytes will error, and attempting to
   allocate more than 128 MiB will error and exit
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "slug.h"

#define MAX_ALLOCATION 2 /* How many allocations */

int main() {
    void *mem[MAX_ALLOCATION] = { 0 };

    mem[0] = malloc(0);

    mem[1] = malloc(128000001);

    printf("We should not get here, as the over 128 MiB\
                      allocation should have exited.\n");
    return 0;
}
