#include <stdio.h>
#include "null_hunter.h"

void count_null(FILE* fp, P_NULL_STATS p_null_stats) {
    char ch = 0;

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
}


