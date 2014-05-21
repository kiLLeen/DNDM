#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "slug.h"

int main (int argc, char** argv) {
  void* aa = NULL;
  int i=0,j=0;
  printf("hey.\n");
  malloc(10);
  
  for (i=1245; i > 0; --i) {
    if (i%123 == 0) {
      for (j=1274*i; j > 0; --j) {}
      malloc(i);
      for (j=123*i; j > 0; --j) {}
      malloc(i+1);
    }
  }

  for (i=4568878; i > 0; --i) {}
  aa = malloc(1119);
  slug_memstats();
  free (aa);
  slug_memstats();
  return(11);
}
