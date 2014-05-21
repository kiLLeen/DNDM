/* CREATED by Desmond Vehar (dvehar) on 5.20.14 */

#ifndef __SLLUG_H__
#define __SLLUG_H__

/* macros that will replace malloc(x) and free(y) calls with 
   slug_malloc(x, FILE:LINENR) and slug_free(y, FILE:LINENR). */
#define FUNCTIONIZE(a,b) a(b)
#define STRINGIZE(a) #a
#define INT2STRING(i) FUNCTIONIZE(STRINGIZE,i)
#define FILE_POS __FILE__ ":" INT2STRING(__LINE__)
#define malloc(s) slug_malloc((s), FILE_POS)
#define free(s) slug_free((s), FILE_POS)

/* function: slug_malloc
 * parameters: size - size of the allocation to make in bytes
 *             WHERE - a string in this format: "filename:linenumber"
 * returns: what a malloc call would return (address to start of the allocated memory)
 * intended action: - to malloc and also keep track of info about the allocation.
 *                  - to crash if malloc fails to allocate memory
 *                  - to register a callback to slug_memstats when the user's program 
 *                    finishes.
 *                  - warn if an alloc of size 0 is requested
 *                  - crash if an alloc over 1048576 is requested
 * assumptions: WHERE is formatted correctly
 */
void *slug_malloc ( size_t size, char *WHERE );

/* function: slug_malloc
 * parameters: addr - the starting address of a memory allocation to free
 *             WHERE - a string in this format: "filename:linenumber"
 * returns: what a malloc call would return (address to start of the allocated memory)
 * intended action: - to free and also keep track of info about the allocation.
 *                  - to crash if the user tries to free memory that has already been free'd
 *                     or the user gives an address that doesn't point to the start of a 
 *                     memory allocation made by them
 * assumptions: WHERE is formatted correctly
 */
void slug_free ( void *addr, char *WHERE );

/* function: slug_malloc
 * parameters: void
 * returns: void
 * intended action: - to print out information about every memory leak
 *                  - to print number of allocations and the total size,
 *                    mean, and standard deviation of the allocations. Both 
 *                    for active allocations (non-free'd) and all allocations.
 * assumptions: the online algorithm we use for calculating the variance has 
 *              precision issues.
 */
void slug_memstats ( void );

#endif
