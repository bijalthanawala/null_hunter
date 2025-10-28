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
}

void test_nulls_in_the_beginning(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputc(0, fp);
    fputc(0, fp);
    fputc(0, fp);
    fputs("ABCD", fp);

    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 3);
    CU_ASSERT_EQUAL(stats.null_segments, 1);
}

void test_nulls_in_the_end(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputs("ABCD", fp);
    fputc(0, fp);
    fputc(0, fp);
    fputc(0, fp);
    fputc(0, fp);
    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 4);
    CU_ASSERT_EQUAL(stats.null_segments, 1);
}
void test_five_null_one_segment_only_in_file(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    for(int i=0; i<5; i++)
        fputc(0, fp);
    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 5);
    CU_ASSERT_EQUAL(stats.null_segments, 1);
}

void test_five_null_one_segment_followed_by_one_nonnull(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    for(int i=0; i<5; i++)
        fputc(0, fp);
    fputc('A', fp);
    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 5);
    CU_ASSERT_EQUAL(stats.null_segments, 1);
}

void test_two_nulls_two_segments_both_segments_at_front_and_end(void)
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
}

void test_two_segments_both_segments_in_between(void)
{
    NULL_STATS stats = {0};
    FILE *fp = tmpfile();
    NH_STATUS status = NH_SUCCESS;

    fputc('A', fp);
    fputc(0, fp);
    fputc(0, fp);
    fputc('B', fp);
    fputc(0, fp);
    fputc(0, fp);
    fputc(0, fp);
    fputc(0, fp);
    fputc('C', fp);


    fseek(fp, SEEK_SET, 0);

    status = null_hunter(fp, &stats);

    fclose(fp);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, 6);
    CU_ASSERT_EQUAL(stats.null_segments, 2);
}

int main()
{
    CU_initialize_registry();

    CU_pSuite null_hunter_tests = CU_add_suite("null_hunter_file_test_suite", NULL, NULL);
    CU_add_test(null_hunter_tests, "NULL file pointer", test_null_fp);
    CU_add_test(null_hunter_tests, "NULL stats pointer", test_null_stats);
    CU_add_test(null_hunter_tests, "zero length file", test_zero_length_file);
    CU_add_test(null_hunter_tests, "no null in file of size one byte", test_no_null_file_size_one);
    CU_add_test(null_hunter_tests, "no null in file of size five bytes", test_no_null_file_size_five);
    CU_add_test(null_hunter_tests, "one null in file of size one byte", test_one_null_file_size_one);

    CU_add_test(null_hunter_tests, "test_nulls_in_the_beginning", test_nulls_in_the_beginning);
    CU_add_test(null_hunter_tests, "test_nulls_in_the_end", test_nulls_in_the_end);

    CU_add_test(null_hunter_tests, "five NULLs in one segment only in a file", test_five_null_one_segment_only_in_file);
    CU_add_test(null_hunter_tests, "five NULLs in one segment followed by one non-null char", test_five_null_one_segment_followed_by_one_nonnull);
    CU_add_test(null_hunter_tests, "test_two_nulls_two_segments_both_segments_at_front_and_end", test_two_nulls_two_segments_both_segments_at_front_and_end);
    CU_add_test(null_hunter_tests, "test_two_segments_both_segments_in_between", test_two_segments_both_segments_in_between);


    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}