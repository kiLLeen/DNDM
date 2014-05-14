#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> 
#include <sys/time.h> 

#define OK 0
#define BAD 1
#define ALLOC_LIMIT 1048576

/* 
 * linked list code 
 ********************/

typedef struct node {
  /* data */
  long int time_sec;
  long int time_usec;
  size_t size;
  char* file_name;
  int line_num;
  void* address;
  /* link */
  struct node* link;
} node;

node* head = NULL;
long int total_allocs = 0;
/*node* tail = NULL;  there is no particuliar reason to treat it like a queue */

int insert_node (int linenr, size_t mem_size, char* file_name, void** address) {
  int ret_val = OK;
  node* tmp_node = NULL;
  struct timeval tmp_time;
  
  if (mem_size == 0) {
    fprintf(stderr, "unusual opperation: allocation of 0 bytes\n");
  } else if (mem_size > ALLOC_LIMIT) {
    fprintf(stderr, "excessive allocation size request of %zu bytes\n", mem_size);
    exit(1);
  } else {
    *address = malloc(mem_size);
    if (*address == NULL) {
      /*ret_val = BAD;*/
      fprintf(stderr, "failed to allocate memory\n");
      exit(1);
    } else {
      ++total_allocs;
      tmp_node = malloc(sizeof(node));
      assert(tmp_node != NULL);
      tmp_node -> size = mem_size;
      tmp_node -> file_name = file_name; /* need to strdup?  TODO */
      tmp_node -> line_num = linenr;
      tmp_node -> address = *address;
      /* get time*/
      gettimeofday(&tmp_time, NULL);
      tmp_node -> time_sec = tmp_time.tv_sec;
      tmp_node -> time_usec = tmp_time.tv_usec;
      printf("timestamp::: %ld\n", tmp_time.tv_usec);
      /*insert the node into the linked list */
      if (head == NULL) {
        tmp_node -> link = NULL;
      } else {
        tmp_node -> link = head;
      }
      head = tmp_node;
    }
  }
  return ret_val;
}

/* 
 * util code 
 ********************/

/* returns the line number
   replaces the ':' with '\0' 
   assumes there is only one colon
   assumes that if only a filename is given that the macro passed us bad info 
   because it is in a loop */
int split (char* filename) {
  int i = 0;
  int ret_val = -1;
  char* new_str;
  printf("in split: %s\n", filename);
  if (filename != NULL) {
    /* find the location of the colon or runoff */
    for (i = strlen(filename) - 1; i >= 0 && filename[i] != ':'; --i) {  
    }
    
    /* don't need to check if we only call the function using the str*/
    if (i >= 0) {
      ret_val = atoi(filename + i + 1);
      new_str = calloc(i+1, sizeof(char));
      memcpy(new_str, filename, (i+1)*sizeof(char));
      /*filename[i] = '\0';       */
      /*printf("str[%s]  linenr[%d]\n", filename, *linenr);*/
    } else { /* This occurs when the macro was in a loop */
      printf("\n\nHELLA BAD\n\n");
    }
  }  
  printf("out split: %s, %d, %d\n", filename, ret_val, i);
  
  return (ret_val); 
}

void *slug_malloc ( size_t size, char *WHERE ) {
  int linenr = 0;
  void* address = NULL;
  printf("in  slug malloc\n");
  printf("+_+_+_+ %s\n", WHERE);
  linenr = split(WHERE);
  if (insert_node(linenr, size, WHERE, &address) == BAD) {
    fprintf(stderr, "--- TODO ---\n");
  }
  printf("out slug malloc\n");
  return address;
}

void slug_free ( void *addr, char *WHERE ) {
  node* curr = head;
  node* tmp = NULL; /* used when removing a node */
  
  printf("in slug free\n");
  
  /* look for the address in the linked list */
  while ((curr != NULL) && (addr != curr -> address)) {
    tmp = curr;
    curr = curr -> link;
  }
  
  if (curr == NULL) { /* not in the list */
    fprintf(stderr, "%s: address %p was not allocated to you\n", WHERE, addr);
    exit(1);
  } else if (tmp == NULL) { /* the head is getting removed */
    head = head -> link;
  } else { /* some node in the 'tail' is getting removed */
    tmp -> link = curr -> link; /* node before curr points to the node after curr */
  }
  free(curr);
  printf("out slug free\n");
}

void slug_memstats ( void ) {
  /* for itterating over the linked list */
  node* curr = head;
  /* for calculating the meterics */
  long int total_size = 0;
  long int alloc_number = 0;
  long int tmp = 0; /* used for calculating the std */
  long int mean = 0;
  long int std = 0;
  
  printf("in  slug memstats\n");
  /* dump each allocations info and calculate the total size allocated */
  for (; curr != NULL; curr = curr -> link, ++alloc_number) {
    printf("address: %p\ntime: %ld.%06ld\nsize: %lu\nfile: %s\nline number: %d\n\n",
            curr->address, curr->time_sec, curr->time_usec,(unsigned long)curr->size, curr->file_name,
            curr->line_num);
    total_size += curr -> size;        
  }

  /* calculate the mean and std only if the number allocated != zero */
  if (alloc_number != 0) {
    /* calculate the mean */
    mean = total_size / alloc_number;
  
    /* calculate the variance */
    for (curr = head; curr != NULL; curr = curr -> link) {
      tmp += pow(curr -> size - mean, 2);
    }
    
    /* calculate the std */
    std = sqrt(tmp / alloc_number);
  }
  
  /* dump meta allocation information */
  printf("total allocations made: %ld\n", total_allocs);
  printf("number of active allocations: %ld\n", alloc_number);
  printf("total bytes currently allocated: %ld\n", total_size);
  printf("mean of memory currently allocated: %ld\n", mean);
  printf("standard deviation of memory currently allocated: %ld\n", std);

  printf("out slug memstats\n");
}
