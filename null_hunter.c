#include <stdio.h>
#include "null_hunter.h"

NH_STATUS null_hunter(FILE* fp, P_NULL_STATS p_null_stats) {
    char ch = 0;

    if(!fp) {
        return NH_ERROR_NULL_FILE_POINTER;
    }

    if(!p_null_stats) {
        return NH_ERROR_NULL_STAT_POINTER;
    }

    p_null_stats->null_segments = 0;
    p_null_stats->total_null_count = 0;

    while(ch != -1) {
        ch = fgetc(fp);

        if(ch == 0)
            p_null_stats->null_segments++;

        while(ch == 0) {
            p_null_stats->total_null_count++;
            ch = fgetc(fp);
        }
    }

    return NH_SUCCESS;
}


