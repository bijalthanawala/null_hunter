#ifndef NULL_HUNTER_H

#define NULL_HUNTER_H

typedef enum {
    NH_SUCCESS,
    NH_ERROR_NULL_FILE_POINTER,
    NH_ERROR_NULL_STAT_POINTER,
    NH_UNKNOWN_ERROR
} NH_STATUS;

typedef struct null_stats {
    unsigned long file_size;
    unsigned long total_null_count;
    unsigned long total_null_segments;
    unsigned long longest_segment_size;
    unsigned long longest_last_segment_offset;
} NULL_STATS, *P_NULL_STATS;

NH_STATUS null_hunter(FILE* fp, P_NULL_STATS p_null_stats);
void util_print_null_stats(P_NULL_STATS p_stats);

#endif