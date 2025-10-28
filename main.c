#include <stdio.h>
#include "null_hunter.h"

int main(int argc, char **argv) {

    FILE *fp = NULL;
    NULL_STATS null_stats = {0};

    if(argc < 2) {
        printf("Please specify the file to look NULL into!\n");
        return -1;
    }

    printf("Scanning file %s\n", argv[1]);
    fp = fopen(argv[1], "rb");
    count_null(fp, &null_stats);

    printf("Found total %d NULL characters in %d segments\n",
        null_stats.total_null_count,
        null_stats.null_segments
    );

    return 0;
}
