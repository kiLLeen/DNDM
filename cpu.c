/* CREATED 4-24-2014 */

#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include <time.h>

/* The main program. Calculates pi as a cpu bound test. */
/* Occationally the program emits the process id and how much work has been done */
/* ARGC is the number of arguments entered when running this program */
/* ARGV is the array of character string arguments entered with this program */
/* returns 0 on exit */
int main(int argc, char *argv[]) {
    /* PI = 4 * (1/1 - 1/3 + 1/5 - 1/7 + - ...) */
    int i;
    double sum = 0;
    int denom, numer;
    time_t current;
    time_t start = time(NULL);
    int seconds = 0;
    pid_t process_id = getpid();

    for (i=1;i<2500000;++i) {
        denom = 2 * i - 1;
        numer;
        if (i % 2)
            numer = -1;
        else
            numer = 1;
        sum += ((double)numer / (double)denom);
        current = time(NULL);
        if (current - start > seconds) {
            seconds = current - start;
            printf("Process %d has been running for %d seconds.\n", process_id, seconds);
            if (seconds == 10)
                break;
        }
    }
    printf("pi calculated as %f\n", -4 * sum);
    return 0;
}
