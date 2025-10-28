#include <stdio.h>

int main(int argc, char **argv) {

    FILE *fp = NULL;
    char ch = 0;
    int count = 0;

    if(argc < 2) {
        printf("Please specify the file to look NULL into!\n");
        return -1;
    }

    printf("Scanning file %s\n", argv[1]);
    fp = fopen(argv[1], "rb");
    while((ch = fgetc(fp)) != -1) {
        if(ch == 0) {
            count++;
        }
    }

    printf("Found %d NULL characters\n", count);

    return 0;
}