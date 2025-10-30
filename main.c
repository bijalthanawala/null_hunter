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

    float total_null_percentage =  (float)(null_stats.total_null_count * 100)/ null_stats.file_size;
    float longest_segment_percentage =  (float)(null_stats.longest_segment_size * 100)/ null_stats.file_size;
    printf("File size = %lu\n", null_stats.file_size);
    printf("Total NULL count = %lu (%.2f%%)\n", null_stats.total_null_count,total_null_percentage);
    printf("Total NULL segments = %lu\n", null_stats.total_null_segments);
    printf("Longest segment size = %lu (%.2f%%)\n", null_stats.longest_segment_size, longest_segment_percentage);
    printf("Offset of the (last) longest segment = %lu\n", null_stats.longest_last_segment_offset);


    return status;
}
