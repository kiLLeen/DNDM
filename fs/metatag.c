/* CREATED 5-27-14 */

#include <lib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    FILE *infile;
    char buff[1025];
    int count, size;

    if (argc != 3) { /* call syntax invalid */
        printf("Usage: metatag filename \"tag\"\n");
        return(-1);
    }
    infile = fopen(argv[1], "r+"); /* must open read write */
    if (infile == NULL) {
        printf("Error opening file\n");
        return(-1);
    }
    size = strlen(argv[2]);
    memcpy(buff, argv[2], size);
    buff[size++] = '\0';
    count = metawrite(infile->_file, buff, size);
    if (count == -1)
        printf("metawrite errored\n");
    else if (size != count)
        printf("Warning: only %d bytes written.\n", count);
    fclose(infile);
    return (count < 0) ? -1 : 0;
}