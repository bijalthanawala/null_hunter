#include <stdio.h>

typedef struct null_stats {
    int total_null_count;
    int null_segments;
} NULL_STATS, *P_NULL_STATS;

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
