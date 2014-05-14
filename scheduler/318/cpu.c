/* CREATED 4-24-2014 */

#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

extern void read_tsc_64(u64_t* t);
extern int atoi(char* str);
extern u64_t sub64(u64_t s, u64_t t);
extern int nice(int n);
extern void exit(void);

/* The main program. Calculates pi as a cpu bound test. */
/* Occationally the program emits the process id and how much work has been done */
/* ARGC is the number of arguments entered when running this program */
/* ARGV is the array of character string arguments entered with this program */
/* returns 0 on exit */
int main(int argc, char *argv[]) {
    /* argument 1 is the number of iterations to run */
    /* argument 2 is the argument to nice() */

    /* PI = 4 * (1/1 - 1/3 + 1/5 - 1/7 + 1/9 - 1/11 ...) */
    int i,j;
    double sum = 0;
    int denom, numer;
    pid_t process_id = getpid();
    int retval, nice_val, iters;
    u64_t s,e,diff;
    double elapsed;
    unsigned long max;

    if (argc < 3) {
        printf("usage: cpu iterations tickets\n");
        return 1;
    }

    iters = atoi(argv[1]);
    nice_val = atoi(argv[2]);
    retval = nice(nice_val);
    if (retval == -1) {
        printf("Error calling nice()\n");
        return 1;
    }

    read_tsc_64(&s);
    for (j = 1; j <= 10; ++j) {
        for (i = 1; i<iters/10; ++i) {
            denom = 2 * i - 1;
            if (i % 2)
                numer = -1;
            else
                numer = 1;
            sum += ((double)numer / (double)denom);
        }
        printf("CPU Process %d has completed %d percent of it's work.\n", process_id, j * 10);
    }
    read_tsc_64(&e);
    diff = sub64(e, s);
    max = -1;
    elapsed = (double)diff.hi + (double)(diff.lo/100000) / (double)(max/100000);
    printf("CPU process %d (nice %d) calculated pi as %f at %f time units\n", process_id, nice_val, -0.4 * sum, elapsed);
    return 0;
}
