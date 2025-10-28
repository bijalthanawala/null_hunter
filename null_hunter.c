#include <stdio.h>

int main(int argc, char **argv) {

    FILE *fp = NULL;
    char ch = 0;
    int null_segments = 0;
    int total_null_count = 0;

    if(argc < 2) {
        printf("Please specify the file to look NULL into!\n");
        return -1;
    }

    printf("Scanning file %s\n", argv[1]);
    fp = fopen(argv[1], "rb");
    while(ch != -1) {
        ch = fgetc(fp);

        if(ch == 0)
            null_segments++;

        while(ch == 0) {
            total_null_count++;
            ch = fgetc(fp);
        }
    }

    printf("Found total %d NULL characters in %d segments\n", total_null_count, null_segments);

    return 0;
}