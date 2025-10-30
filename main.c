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

    printf("Found total %lu NULL characters in %lu segments, with longest segment of %lu characters at offset %lu\n",
        null_stats.total_null_count,
        null_stats.total_null_segments,
        null_stats.longest_segment_size,
        null_stats.longest_last_segment_offset
    );

    return status;
}
