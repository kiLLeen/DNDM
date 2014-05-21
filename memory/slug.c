/* CREATED by Desmond Vehar (dvehar) on 5.20.14 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> 
#include <sys/time.h> 
#include <inttypes.h>

#define OK 0
#define BAD 1
#define ALLOC_LIMIT 1048576

/*#define SLUG_DEBUG*/

void slug_memstats ( void );

typedef struct node {
  /* data */
  long int time_sec;
  long int time_usec;
  size_t size;
  char* file_name;
  long int line_num;
  void* address; /* the mem that was malloc'd */
  /* link */
  struct node* link;
} node;

node* head = NULL;

/* for calculating the active metrics */
uintmax_t active_total_size = 0;
uintmax_t active_alloc_count = 0;
long double active_mean = 0;
long double active_m2 = 0;
/* for calculating the total metrics */
uintmax_t total_size_allocated = 0;
uintmax_t total_alloc_count = 0;
long double total_mean = 0;
long double total_m2 = 0;

/* function name: insert_node
 * parameters: linenr - linenumber from the file the slug_malloc occured on
 *             mem_size - how much memory to malloc in bytes
 *             file - filename of where the slug_malloc call was made
 *             address - a pointer to the location where the address 
 *                       returned from malloc should be stored
 * returns: OK
 * intended action: - to malloc and also keep track of info about the allocation.
 *                  - to crash if malloc fails to allocate memory
 *                  - to register a callback to slug_memstats when the user's program 
 *                    finishes.
 *                  - warn if an alloc of size 0 is requested
 *                  - crash if an alloc over 1048576 is requested
 * assumptions: none
 */
int insert_node (int linenr, size_t mem_size, char* file_name, void** address) {
  int ret_val = OK;
  node* tmp_node = NULL;
  struct timeval tmp_time;
  long double delta = 0;
  
  if (mem_size > ALLOC_LIMIT) {
    fprintf(stderr, "excessive allocation size request of %zu bytes\n", mem_size);
    exit(1);
  } else {
    if (mem_size == 0)
      fprintf(stderr, "unusual opperation: %s: %d: allocation of 0 bytes\n", file_name, linenr);      

    *address = malloc(mem_size);
    if (*address == NULL && mem_size != 0) {
      /*ret_val = BAD;*/
      fprintf(stderr, "failed to allocate memory\n");
      exit(1);
    } else {
      tmp_node = malloc(sizeof(node));
      assert(tmp_node != NULL);
      /* update total stats */
      ++total_alloc_count;
      total_size_allocated += mem_size;
      delta = mem_size - total_mean;
      total_mean += delta / total_alloc_count;
      total_m2 += delta * (mem_size - total_mean);
      /* update active stats */
      ++active_alloc_count;
      active_total_size += mem_size;
      delta = mem_size - active_mean;
      active_mean += delta / active_alloc_count;
      active_m2 += delta * (mem_size - active_mean);
      /* set the node's values approperiately */
      tmp_node -> size = mem_size;
      tmp_node -> file_name = file_name;
      tmp_node -> line_num = linenr;
      tmp_node -> address = *address;
      /* get time*/
      gettimeofday(&tmp_time, NULL);
      tmp_node -> time_sec = tmp_time.tv_sec;
      tmp_node -> time_usec = tmp_time.tv_usec;
      /*insert the node into the linked list */
      if (head == NULL) {
        tmp_node -> link = NULL;
        if (total_alloc_count == 1) {
          if (atexit(slug_memstats) != 0) { /* set up the callback for when program exits */
            fprintf(stderr, "Unable to setup the memory statistics callback.\n");
            exit(1);
          }
        }
      } else {
        tmp_node -> link = head;
      }
      head = tmp_node;
    }
  }
  return ret_val;
}

/* function name: split
 * parameters: str - something in "filename:linenr" format
 *             filename - a pointer to a location which will be modified to
 *                        hold a pointer to the extracted filename (strdup'd)
 * returns: the extracted line number
 * intended action: to parse the str
 * assumptions: str is in the appropriate format (there is at least one character 
 *              before the colon and one number after)
 */
int split (char* str, char** filename) {
  int i = 0;
  int ret_val = -1;
  char* new_str;

#ifdef SLUG_DEBUG  
  printf("in split: %s\n", str);
#endif
  
  /* find the location of the colon or runoff */
  for (i = strlen(str) - 1; i >= 0 && str[i] != ':'; --i) {  
  }
  
  /* don't need to check if we only call the function using the str*/
  ret_val = atoi(str + i + 1);
  new_str = calloc(i+1, sizeof(char));
  memcpy(new_str, str, i*sizeof(char));
  *filename = new_str;

#ifdef SLUG_DEBUG  
  printf("out split: %s, %d, %d\n", str, ret_val, i);
#endif
  
  return (ret_val); 
}

void *slug_malloc ( size_t size, char *WHERE ) {
  int linenr = 0;
  void* address = NULL;
  char* filename = NULL;

#ifdef SLUG_DEBUG  
  printf("in  slug malloc\n");
#endif
  
  linenr = split(WHERE, &filename);
  if (insert_node(linenr, size, filename, &address) == BAD) {
    /* fprintf(stderr, "--- TODO ---\n"); */
  }

#ifdef SLUG_DEBUG  
  printf("out slug malloc\n");
#endif
  
  return address;
}

void slug_free ( void *addr, char *WHERE ) {
  node* curr = head;
  node* tmp = NULL; /* used when removing a node */
  long double delta = 0;
#ifdef SLUG_DEBUG  
  printf("in slug free\n");
#endif
 
  /* look for the address in the linked list */
  while ((curr != NULL) && (addr != curr -> address)) {
    tmp = curr;
    curr = curr -> link;
  }
 
  if (curr == NULL) { /* not in the list */
    fprintf(stderr, "%s: address %p is not the first byte of any memory that %s\n", 
                   "was allocated to you", WHERE, addr);
    exit(1);
  } else if (tmp == NULL) { /* the head is getting removed */
    head = head -> link;
  } else { /* some node in the 'tail' is getting removed */
    tmp -> link = curr -> link; /* node before curr points to the node after curr */
  }
  
  /* update the active stats */
  active_total_size -= curr -> size;
  --active_alloc_count;
  delta = curr -> size - active_mean;
  active_mean -= delta / active_alloc_count;
  active_m2 -= delta * (curr -> size - active_mean);
 
  /* free the node data */
  free(curr -> address);
  free(curr -> file_name);
  free(curr);
#ifdef SLUG_DEBUG  
  printf("out slug free\n");
#endif  
}

void slug_memstats ( void ) {
  node* curr = head; /* for itterating over the linked list */

#ifdef SLUG_DEBUG  
  printf("in slug memstats\n");
#endif
  
  /* dump each allocations info and calculate the total size allocated */
  for ( ; curr != NULL ; curr = curr -> link) {
    printf("address: %p\ntime: %ld.%06ld\nsize: %lu\nfile: %s\nline number: %ld\n-   -   -   -   -   -  -\n",
           curr->address, curr->time_sec, curr->time_usec,(unsigned long)curr->size, curr->file_name,
           curr->line_num);
  }
   
  if (active_alloc_count != 0) {
    printf("number of active allocations: %" PRIuMAX "\n", active_alloc_count);
    printf("total bytes currently allocated: %" PRIuMAX "\n", active_total_size);
    printf("mean of memory currently allocated: %lg\n", active_mean);
    if (active_alloc_count > 1) {
      printf("standard deviation of memory currently allocated: %lg\n",
             sqrt(active_m2 / (active_alloc_count - 1)));
    } else {
      printf("standard deviation of memory currently allocated: 0\n");
    }
  } else {
    printf("No memory leaks found.\n");
  }
  
  printf("\n");
  printf("total allocations made: %" PRIuMAX "\n", total_alloc_count);
  printf("total bytes allocated: %" PRIuMAX "\n", total_size_allocated);
  printf("mean of memory allocated: %lg\n", total_mean);  
  if (total_alloc_count > 1) {
    printf("standard deviation of memory allocated: %lg\n",
           sqrt(total_m2 / (total_alloc_count - 1)));
  } else {
    printf("standard deviation of memory allocated: 0\n");
  }

#ifdef SLUG_DEBUG
  printf("out slug memstats\n");
#endif  
}
