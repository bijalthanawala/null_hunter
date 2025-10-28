#include <stdio.h>
#include "null_hunter.h"

int main(int argc, char **argv) {

    NH_STATUS status = NH_SUCCESS;
    FILE *fp = NULL;
    NULL_STATS null_stats = {};

    if(argc < 2) {
        printf("Please specify the file to look NULL into!\n");
        return -1;
    }

    printf("Scanning file %s\n", argv[1]);
    fp = fopen(argv[1], "rb");
    status = null_hunter(fp, &null_stats);

    if(status != NH_SUCCESS) {
        printf("Error encountered while counting. Error code=%d\n", status);
        return status;
    }

    printf("Found total %d NULL characters in %d segments\n",
        null_stats.total_null_count,
        null_stats.null_segments
    );

    return status;
}
