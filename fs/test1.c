
#include <lib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    FILE *infile;
    char buff[1025];
    int count;
    
    if (argc != 2) {
        printf("Usage: metacat filename\n");
        return(-1);
    }
    infile = fopen(argv[1], "rb");
    if (infile == NULL) {
        printf("Error opening file\n");
        return(-1);
    }
    count = metaread(infile->_file, buff, 1024);
    fclose(infile);

    printf("we read:\n%s\n", buff);
    return 0;
}