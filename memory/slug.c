/* CREATED by Desmond Vehar (dvehar) on 5.20.14 */

/*
Assignment: Project 3 - Memory Allocation
Class: CMPS111 - Spring 2014
Professor: N. Whitehead

Melanie Dickinson - mldickin
Neil Killeen - nkilleen
Dave Lazell - dlazell
Desmond Vehar - dvehar


This program creates a wrapper around malloc() and free()
By including slug.h and linking the the object file created
this file, user programs upon exit get a report on allocated
memory. This is useful to detect memory leaks. Also, errors
such as freeing already freed memory, or invalid memory, are
reported.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> 
#include <sys/time.h> 
#include <inttypes.h>

#define OK 0
#define BAD 1
#define ALLOC_LIMIT 128000000

void slug_memstats ( void );

typedef struct node {
  /* data */
  long int time_sec;    /* the current timestamp */
  long int time_usec;   /* the current timestamp */
  size_t size;          /* the length of the allocated memory */
  char* file_name;      /* the file in which the malloc was made */
  long int line_num;    /* the line number in the file in which the malloc was made */
  void* address;        /* the address of the allocated memory */
  /* link */
  struct node* link;
} node;

static node* head = NULL;

/* for calculating the active metrics */
static uintmax_t active_total_size = 0;
static uintmax_t active_alloc_count = 0;
static long double active_mean = 0;
static long double active_m2 = 0;

/* for calculating the total metrics */
static uintmax_t total_size_allocated = 0;
static uintmax_t total_alloc_count = 0;
static long double total_mean = 0;
static long double total_m2 = 0;

/* function name: insert_node
 * parameters: linenr - linenumber from the file the slug_malloc occurred on
 *             mem_size - how much memory to malloc in bytes
 *             file_name - filename of where the slug_malloc call was made
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
    fprintf(stderr, "Excessive allocation size request of %d bytes\n", (int)mem_size);
    exit(1);
  } else {
    if (mem_size == 0)
      fprintf(stderr, "Unusual operation: %s: %d: allocation of 0 bytes\n", file_name, linenr);      

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
      active_m2 += delta * (mem_size - total_mean);

      /* set the node's values appropriately */
      tmp_node -> size = mem_size;
      tmp_node -> file_name = file_name;
      tmp_node -> line_num = linenr;
      tmp_node -> address = *address;

      /* get time*/
      gettimeofday(&tmp_time, NULL);
      tmp_node -> time_sec = tmp_time.tv_sec;
      tmp_node -> time_usec = tmp_time.tv_usec;

      /* insert the node into the linked list */
      if (head == NULL) {
        tmp_node -> link = NULL;
        
	    /* set up the callback for when program exits */
	    if (total_alloc_count == 1 && atexit(slug_memstats) != 0) { 
          fprintf(stderr, "Unable to setup the memory statistics callback.\n");
          exit(1);
        }

      } else
        tmp_node -> link = head;
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

  /* find the location of the colon or runoff */
  for (i = strlen(str) - 1; i >= 0 && str[i] != ':'; --i);
  
  /* don't need to check if we only call the function using the str */
  ret_val = atoi(str + i + 1);
  new_str = calloc(i+1, sizeof(char));
  memcpy(new_str, str, i*sizeof(char));
  *filename = new_str;

  return (ret_val); 
}

/* see slug.h for function info */
void *slug_malloc ( size_t size, char *WHERE ) {
  int linenr = 0;
  void* address = NULL;
  char* filename = NULL;

  linenr = split(WHERE, &filename);
  insert_node(linenr, size, filename, &address);
  
  return address;
}

/* see slug.h for function info */
void slug_free(void *addr, char *WHERE) {
  node* curr = head;
  node* tmp = NULL; /* used when removing a node */
  long double delta = 0;
 
  /* look for the address in the linked list */
  while ((curr != NULL) && (addr != curr -> address)) {
    tmp = curr;
    curr = curr -> link;
  }
 
  if (curr == NULL) { /* not in the list */
    fprintf(stderr, "%s: address %p is not the first byte of any memory that \
was allocated to you\n", WHERE, addr);
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
  curr->address = NULL;
  free(curr -> file_name);
  curr->file_name = NULL;
  free(curr);
  curr = NULL;
}

/* see slug.h for function info */
void slug_memstats(void) {
  node* curr = head; /* for iterating over the linked list */
  node* tmp = NULL;

  /* dump each allocations info and calculate the total size allocated */
  while(curr != NULL) {

    printf("address: %p\ntime: %ld.%06ld\nsize: %lu\nfile: \
%s\nline number: %ld\n-   -   -   -   -   -  -\n",
           curr->address, 
	   curr->time_sec, 
	   curr->time_usec,
	   (unsigned long)curr->size, 
	   curr->file_name,
           curr->line_num);

    tmp = curr->link;
    free(curr->address); /* free leaked memory from user program */
    curr->address = NULL;
    free(curr->file_name);
    curr->file_name = NULL;
    free(curr);
    curr = tmp;
  }
   
  if (active_alloc_count != 0) {
    printf("Number of active allocations: %" PRIuMAX "\n", active_alloc_count);
    printf("Total bytes currently allocated: %" PRIuMAX "\n", active_total_size);
    printf("Mean of memory sizes currently allocated: %Lg\n", active_mean);

    if (active_alloc_count > 1)
      printf("Standard deviation of memory sizes currently allocated: %lg\n",
             sqrt(active_m2 / (active_alloc_count - 1)));
    else
      printf("Standard deviation of memory sizes currently allocated: 0\n");

  } else
    printf("No memory leaks found.\n");
  
  printf("\n");
  printf("Total allocations made: %" PRIuMAX "\n", total_alloc_count);
  printf("Total bytes allocated: %" PRIuMAX "\n", total_size_allocated);
  printf("Mean of memory sizes allocated: %Lg\n", total_mean);  
  if (total_alloc_count > 1)
    printf("Standard deviation of memory sizes allocated: %lg\n",
           sqrt(total_m2 / (total_alloc_count - 1)));
  else
    printf("Standard deviation of memory sizes allocated: 0\n");
}
