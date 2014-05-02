/* CREATED 4-24-2014 */

#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include <minix/minlib.h>

extern void read_tsc_64(u64_t* t);
extern int atoi(char* str);
extern u64_t sub64(u64_t s, u64_t t);
extern int nice(int n);

/* This test program simulates an IO bound process with some minor CPU use. */
/* Opens a file (proc.c), adds up every byte into a checksum, then saves the
   input data back into a new copy of the file (test.txt) and print out the checksum
   (reading and writing blocks of 1024 bytes). */
/* Occationally the program emits the process id and how much work has been done */
/* ARGC is the number of arguments entered when running this program */
/* ARGV is the array of character string arguments entered with this program */
/* returns 0 on exit */
int main(int argc, char *argv[]) {
    /* argument 1 is the number of iterations to run */
    /* argument 2 is the argument to nice() */
    int i, j, k;
    pid_t process_id = getpid();
    FILE *infile, *outfile;
    char buff[1025];
    size_t count, count2;
    int checksum = 0;
    int block = 0;
    int err;
    int retval, nice_val, iters;
    u64_t s, e, elapsed;

    if (argc < 3) {
        printf("usage: io iterations tickets\n");
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
    for (k = 1; k <= 10; ++k) {
        for (i = 1; i<iters/10; ++i) {
            infile = fopen("schedule.c", "rb");
            /* this only writes the current block, but is sufficient for testing purposes */
            outfile = fopen("test.txt", "wb");
            err = fseek(infile, block * 1024, SEEK_SET);
            fseek(outfile, block * 1024, SEEK_SET);
            count = fread(buff, 1, 1024, infile);
            for (j = 0; j < count; ++j)
                checksum += buff[j];
            count2 = fwrite(buff, 1, count, outfile);
            fclose(infile);
            fclose(outfile);
            block++;
            if (count < 1024) {
                block = 0;
                checksum = 0;
            }
        }
        printf("IO Process %d has completed %d percent of it's work.\n", process_id, k * 10);
    }
    read_tsc_64(&e);
    printf("IO process %d (nice %d) completed at %f time units\n", process_id, nice_val, (double)(e - s) / 1000000);
    remove("test.txt");
    return 0;
}
