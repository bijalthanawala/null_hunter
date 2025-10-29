#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include <stdlib.h>   // For mkdtemp()
#include <unistd.h>  // For rmdir()

#include "null_hunter.h"

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
    CU_ASSERT_EQUAL(stats.null_segments, 0);
    CU_ASSERT_EQUAL(stats.max_segment_size, 0);
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
    CU_ASSERT_EQUAL(stats.null_segments, 0);
    CU_ASSERT_EQUAL(stats.max_segment_size, 0);
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
    CU_ASSERT_EQUAL(stats.null_segments, 0);
    CU_ASSERT_EQUAL(stats.max_segment_size, 0);
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
    CU_ASSERT_EQUAL(stats.null_segments, 1);
    CU_ASSERT_EQUAL(stats.max_segment_size, 1);
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
    CU_ASSERT_EQUAL(stats.null_segments, 1);
    CU_ASSERT_EQUAL(stats.max_segment_size, 3);
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
    CU_ASSERT_EQUAL(stats.null_segments, 1);
    CU_ASSERT_EQUAL(stats.max_segment_size, 4);
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
    CU_ASSERT_EQUAL(stats.null_segments, 1);
    CU_ASSERT_EQUAL(stats.max_segment_size, 5);
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
    CU_ASSERT_EQUAL(stats.null_segments, 1);
    CU_ASSERT_EQUAL(stats.max_segment_size, 5);
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
    CU_ASSERT_EQUAL(stats.null_segments, 2);
    CU_ASSERT_EQUAL(stats.max_segment_size, 1);
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
    CU_ASSERT_EQUAL(stats.null_segments, 2);
    CU_ASSERT_EQUAL(stats.max_segment_size, 4);
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

    util_print_null_stats(&stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 9);
    CU_ASSERT_EQUAL(stats.null_segments, 2);
    CU_ASSERT_EQUAL(stats.max_segment_size, 6);
}


int main()
{
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

    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}