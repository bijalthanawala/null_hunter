#include <stdio.h>
#include <memory.h>
#include "null_hunter.h"

NH_STATUS null_hunter(FILE* fp, P_NULL_STATS p_null_stats) {
    int ch = 0;
    unsigned long this_segment_size = 0;
    unsigned long longest_segment_size = 0;
    unsigned long longest_last_segment_offset = 0;
    unsigned long current_offset = 0;
    unsigned long offset_start_of_null_segment = 0;

    if(!fp) {
        return NH_ERROR_NULL_FILE_POINTER;
    }

    if(!p_null_stats) {
        return NH_ERROR_NULL_STAT_POINTER;
    }

    // Don't assume that caller has initialized the stats
    memset(p_null_stats, 0, sizeof(NULL_STATS));

    // Determine the file size
    fseek(fp, 0, SEEK_END);
    p_null_stats->file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    while(ch != -1) {

        ch = fgetc(fp);
        current_offset++;
        this_segment_size = 0;

        if(ch == 0) {
            p_null_stats->total_null_segments++;
            offset_start_of_null_segment = current_offset-1;
        }

        while(ch == 0) {
            p_null_stats->total_null_count++;
            this_segment_size++;
            ch = fgetc(fp);
            current_offset++;
        }

        // If we just observed a NULL segment
        if(this_segment_size) {

            // If this the longest NULL segment seen so far
            if(this_segment_size >= longest_segment_size) {
                longest_segment_size = this_segment_size;
                //longest_last_segment_offset = current_offset-this_segment_size-1;
                longest_last_segment_offset = offset_start_of_null_segment;
            }

            // If this NULL segment has appeared at the very end of the file
            if((offset_start_of_null_segment+this_segment_size) == p_null_stats->file_size) {
                p_null_stats->length_of_null_segment_at_file_end = this_segment_size;
           }
        }

    } // End: while loop

    p_null_stats->longest_null_segment_size = longest_segment_size;
    p_null_stats->longest_last_null_segment_offset = longest_last_segment_offset;

    return NH_SUCCESS;
}

void util_print_null_stats(P_NULL_STATS p_stats) {
    printf("pstats @ %p:\n", (void *)p_stats);
    printf("File size = %lu\n", p_stats->file_size);
    printf("total_null_count = %lu\n", p_stats->total_null_count);
    printf("null_segments = %lu\n", p_stats->total_null_segments);
    printf("longest_segment_size = %lu\n", p_stats->longest_null_segment_size);
    printf("longest_last_segment_offset = %lu\n", p_stats->longest_last_null_segment_offset);
    printf("length_of_null_segment_at_file_end = %lu\n", p_stats->length_of_null_segment_at_file_end);
}