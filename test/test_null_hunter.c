#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include <stdlib.h>   // For mkdtemp()
#include <unistd.h>  // For rmdir()

#include "../null_hunter.h"

#define CU_SUITE_SETUP_SUCCESS 0
#define CU_SUITE_SETUP_ERROR 1
#define CU_SUITE_CLEANUP_SUCCESS CU_SUITE_SETUP_SUCCESS
#define CU_SUITE_CLEANUP_ERROR CU_SUITE_SETUP_ERROR

char temp_dir_path[] = "/tmp/test_null_hunter_XXXXXX";

void test_null_fp(void)
{
    NULL_STATS null_stats = {0};
    NH_STATUS status = null_hunter(NULL, &null_stats);
    CU_ASSERT_EQUAL(status, NH_ERROR_NULL_FILE_POINTER);
}


void test_null_stats(void)
{
    FILE *fp = tmpfile();
    NH_STATUS status = null_hunter(fp, NULL);
    fclose(fp);
    CU_ASSERT_EQUAL(status, NH_ERROR_NULL_STAT_POINTER);
}

void test_zero_length_file(void)
{
    NULL_STATS stats = {0};
    FILE *temp_file = tmpfile();
    NH_STATUS status = null_hunter(temp_file, &stats);
    fclose(temp_file);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 0);
    CU_ASSERT_EQUAL(stats.total_null_segments, 0);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 0);
}

void test_no_null_file_size_one(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputc('A', fp);
    status = null_hunter(fp, &stats);
    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 0);
    CU_ASSERT_EQUAL(stats.total_null_segments, 0);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 0);
}

void test_no_null_file_size_five(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputs("ABCDE", fp);
    status = null_hunter(fp, &stats);
    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 0);
    CU_ASSERT_EQUAL(stats.total_null_segments, 0);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 0);
}

void test_one_null_file_size_one(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputc(0, fp);
    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 1);
    CU_ASSERT_EQUAL(stats.total_null_segments, 1);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 1);
}

void test_nulls_in_the_beginning(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fwrite("\0\0\0", 3, 1, fp);
    fputs("ABCD", fp);

    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 3);
    CU_ASSERT_EQUAL(stats.total_null_segments, 1);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 3);
}

void test_nulls_in_the_end(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputs("ABCD", fp);
    fwrite("\0\0\0\0", 4, 1, fp);
    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 4);
    CU_ASSERT_EQUAL(stats.total_null_segments, 1);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 4);
}
void test_five_null_one_segment_only_in_file(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fwrite("\0\0\0\0\0", 5, 1, fp);
    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 5);
    CU_ASSERT_EQUAL(stats.total_null_segments, 1);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 5);
}

void test_five_null_one_segment_followed_by_one_nonnull(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fwrite("\0\0\0\0\0", 5, 1, fp);
    fputc('A', fp);
    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 5);
    CU_ASSERT_EQUAL(stats.total_null_segments, 1);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 5);
}

void test_two_nulls_two_segments_both_segments_at_each_end(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputc(0, fp);
    fputc('A', fp);
    fputc(0, fp);

    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 2);
    CU_ASSERT_EQUAL(stats.total_null_segments, 2);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 1);
}

void test_two_segments_both_segments_in_between(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputc('A', fp);
    fwrite("\0\0", 2, 1, fp);
    fputc('B', fp);
    fwrite("\0\0\0\0", 4, 1, fp);
    fputc('C', fp);

    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 6);
    CU_ASSERT_EQUAL(stats.total_null_segments, 2);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 4);
}

void test_char_0xff_in_between(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputc('A', fp);
    fwrite("\0\0\0", 3, 1, fp);
    fputc(0xff, fp);
    fwrite("\0\0\0\0\0\0", 6, 1, fp);
    fputc('C', fp);

    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 9);
    CU_ASSERT_EQUAL(stats.total_null_segments, 2);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 6);
}

void test_offset_of_longest_segment(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputs("ABCDEFGHIJ", fp);          // 10 Characters

    fwrite("\0\0\0", 3, 1, fp);       // 3 NULLs
    fputs("1234567", fp);             // 7 Characters

    fwrite("\0\0", 2, 1, fp);         // 2 NULLs
    fputs("12345678", fp);            // 8 Characters

    fwrite("\0\0\0\0\0", 5, 1, fp);   // 5 NULLs      <--- Offset 30
    fputs("12345", fp);               // 5 Characters

    fwrite("\0", 1, 1, fp);           // 1 NULLs
    fputs("123456789", fp);           // 9 Characters

    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 11);
    CU_ASSERT_EQUAL(stats.total_null_segments, 4);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 5);
    CU_ASSERT_EQUAL(stats.longest_last_segment_offset, 30);
}

void test_offset_of_last_longest_segment(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputs("ABCDEFGHIJ", fp);            // 10 Characters

    fwrite("\0\0\0", 3, 1, fp);         // 3 NULLs
    fputs("1234567", fp);               // 7 Characters

    fwrite("\0\0", 2, 1, fp);           // 2 NULLs
    fputs("12345678", fp);              // 8 Characters

    fwrite("\0\0\0\0\0\0", 6, 1, fp);   // 6 NULLs      <--- Offset 30
    fputs("1234", fp);                  // 4 Characters

    fwrite("\0", 1, 1, fp);             // 1 NULLs
    fputs("123456789", fp);             // 9 Characters

    fwrite("\0\0", 2, 1, fp);           // 2 NULLs
    fputs("12345678", fp);              // 8 Characters

    fwrite("\0\0\0\0\0\0", 6, 1, fp);   // 6 NULLs      <--- Offset 60
    fputs("1234", fp);                  // 4 Characters

    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 20);
    CU_ASSERT_EQUAL(stats.total_null_segments, 6);
    CU_ASSERT_EQUAL(stats.longest_segment_size, 6);
    CU_ASSERT_EQUAL(stats.longest_last_segment_offset, 60);
}

void test_3GB_file_all_zeroes() {
    char buffer[3 * 1024 * 1024] = {}; //3 MB
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    for(int i=0; i<1024; i++) {
        fwrite(buffer, sizeof(buffer), 1, fp);
    }

    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, (unsigned)3 * 1024 * 1024 * 1024);
    CU_ASSERT_EQUAL(stats.total_null_segments, 1);
    CU_ASSERT_EQUAL(stats.longest_segment_size, (unsigned)3 * 1024 * 1024 * 1024);
    CU_ASSERT_EQUAL(stats.longest_last_segment_offset, 0);
}

void test_3GB_file_two_segments() {
    char buffer[3 * 1024 * 1024] = {}; //3 MB
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    for(int i=0; i<1024; i++) {
        fwrite(buffer, sizeof(buffer), 1, fp);
    }

    fseek(fp, 99, SEEK_SET);
    fputc('A', fp);

    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, ((unsigned)3 * 1024 * 1024 * 1024) - 1);
    CU_ASSERT_EQUAL(stats.total_null_segments, 2);
    CU_ASSERT_EQUAL(stats.longest_segment_size, ((unsigned)3 * 1024 * 1024 * 1024) - 100);
    CU_ASSERT_EQUAL(stats.longest_last_segment_offset, 100);
}

void test_4GB_file_all_zeroes() {
    char buffer[4 * 1024 * 1024] = {}; //4 MB
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    for(int i=0; i<1024; i++) {
        fwrite(buffer, sizeof(buffer), 1, fp);
    }

    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, (unsigned long)4 * 1024 * 1024 * 1024);
    CU_ASSERT_EQUAL(stats.total_null_segments, 1);
    CU_ASSERT_EQUAL(stats.longest_segment_size, (unsigned long)4 * 1024 * 1024 * 1024);
    CU_ASSERT_EQUAL(stats.longest_last_segment_offset, 0);
}

void test_4GB_file_two_segments() {
    char buffer[4 * 1024 * 1024] = {}; //4 MB
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    for(int i=0; i<1024; i++) {
        fwrite(buffer, sizeof(buffer), 1, fp);
    }

    fseek(fp, 199, SEEK_SET);
    fputc('A', fp);

    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, ((unsigned)4 * 1024 * 1024 * 1024) - 1);
    CU_ASSERT_EQUAL(stats.total_null_segments, 2);
    CU_ASSERT_EQUAL(stats.longest_segment_size, ((unsigned)4 * 1024 * 1024 * 1024) - 200);
    CU_ASSERT_EQUAL(stats.longest_last_segment_offset, 200);
}

int main()
{
    int num_failed_tests = 0;
    CU_ErrorCode cu_error = CUE_SUCCESS;

    CU_initialize_registry();

    CU_pSuite nh_test_suite = CU_add_suite("null_hunter_file_test_suite", NULL, NULL);
    CU_ADD_TEST(nh_test_suite, test_null_fp);
    CU_ADD_TEST(nh_test_suite, test_null_stats);
    CU_ADD_TEST(nh_test_suite, test_zero_length_file);
    CU_ADD_TEST(nh_test_suite, test_no_null_file_size_one);
    CU_ADD_TEST(nh_test_suite, test_no_null_file_size_five);
    CU_ADD_TEST(nh_test_suite, test_one_null_file_size_one);
    CU_ADD_TEST(nh_test_suite, test_nulls_in_the_beginning);
    CU_ADD_TEST(nh_test_suite, test_nulls_in_the_end);
    CU_ADD_TEST(nh_test_suite, test_five_null_one_segment_only_in_file);
    CU_ADD_TEST(nh_test_suite, test_five_null_one_segment_followed_by_one_nonnull);
    CU_ADD_TEST(nh_test_suite, test_two_nulls_two_segments_both_segments_at_each_end);
    CU_ADD_TEST(nh_test_suite, test_two_segments_both_segments_in_between);
    CU_ADD_TEST(nh_test_suite, test_char_0xff_in_between);
    CU_ADD_TEST(nh_test_suite, test_offset_of_longest_segment);
    CU_ADD_TEST(nh_test_suite, test_offset_of_last_longest_segment);
    CU_ADD_TEST(nh_test_suite, test_3GB_file_all_zeroes);
    CU_ADD_TEST(nh_test_suite, test_3GB_file_two_segments);
    CU_ADD_TEST(nh_test_suite, test_4GB_file_all_zeroes);
    CU_ADD_TEST(nh_test_suite, test_4GB_file_two_segments);

    CU_basic_set_mode(CU_BRM_VERBOSE);

    CU_basic_run_tests();

    cu_error = CU_get_error();

    num_failed_tests = CU_get_number_of_tests_failed();
    if(num_failed_tests)
        printf("Number of failed tests = %d\n", num_failed_tests);


    CU_cleanup_registry();

    if(num_failed_tests || cu_error!=CUE_SUCCESS)
        return 1;    // Report error

    return 0;  // Report success
}