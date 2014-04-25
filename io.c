/* CREATED 4-24-2014 */

#define _POSIX_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include <time.h>

/* This test program simulates an IO bound process with some minor CPU use. */
/* Opens a file (proc.c), adds up every byte into a checksum, then saves the
   input data back into a new copy of the file (test.txt) and print out the checksum
   (reading and writing blocks of 1024 bytes). */
/* Occationally the program emits the process id and how much work has been done */
/* ARGC is the number of arguments entered when running this program */
/* ARGV is the array of character string arguments entered with this program */
/* returns 0 on exit */
int main(int argc, char *argv[]) {
    int i,j;
    time_t current;
    time_t start = time(NULL);
    int seconds = 0;
    pid_t process_id = getpid();
    FILE * infile, * outfile;
    char buff[1025];
    size_t count, count2;
    int checksum = 0;
    int block = 0;
    int err;

    for (i = 1; i<5000; ++i) {
        infile = fopen("proc.c", "rb");
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
        current = time(NULL);
        if (current - start > seconds) {
            seconds = current - start;
            printf("Process %d has been running for %d seconds.\n", process_id, seconds);
            if (seconds == 10)
                break;
        }
    }
    remove("test.txt");
    return 0;
}
