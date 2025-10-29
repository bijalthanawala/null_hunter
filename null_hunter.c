#include <stdio.h>
#include <memory.h>
#include "null_hunter.h"

NH_STATUS null_hunter(FILE* fp, P_NULL_STATS p_null_stats) {
    int ch = 0;
    int this_seg_size = 0;
    int max_seg_size = 0;
    int last_max_seg_offset = 0;
    int curr_offset = 0;

    if(!fp) {
        return NH_ERROR_NULL_FILE_POINTER;
    }

    if(!p_null_stats) {
        return NH_ERROR_NULL_STAT_POINTER;
    }

    // Don't assume that caller has initialized the stats
    memset(p_null_stats, 0, sizeof(NULL_STATS));

    while(ch != -1) {

        ch = fgetc(fp);
        curr_offset++;
        this_seg_size = 0;


        if(ch == 0) {
            p_null_stats->null_segments++;
        }

        while(ch == 0) {
            p_null_stats->total_null_count++;
            this_seg_size++;
            ch = fgetc(fp);
            curr_offset++;
        }

        if(this_seg_size >= max_seg_size) {
            last_max_seg_offset = curr_offset-this_seg_size-1;
            max_seg_size = this_seg_size;
        }

    } // End: while loop

    p_null_stats->max_segment_size = max_seg_size;
    p_null_stats->last_max_segment_offset = last_max_seg_offset;

    return NH_SUCCESS;
}

void util_print_null_stats(P_NULL_STATS p_stats) {
    printf("pstats @ %p:\n", (void *)p_stats);
    printf("total_null_count = %d\n", p_stats->total_null_count);
    printf("null_segments = %d\n", p_stats->null_segments);
    printf("max_segment_size = %d\n", p_stats->max_segment_size);
}

