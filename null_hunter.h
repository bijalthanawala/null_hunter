#ifndef NULL_HUNTER_H

#define NULL_HUNTER_H

typedef struct null_stats {
    int total_null_count;
    int null_segments;
} NULL_STATS, *P_NULL_STATS;

void count_null(FILE* fp, P_NULL_STATS p_null_stats);

#endif