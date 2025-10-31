#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include <stdlib.h>   // For mkdtemp()
#include <unistd.h>  // For rmdir()

#include "../null_hunter.h"

#define SUCCESS 0
#define ERROR 1

FILE *fp_3GB = NULL;
FILE *fp_4GB = NULL;

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
    NULL_STATS stats = {0};
    NH_STATUS status = NH_SUCCESS;

    fseek(fp_3GB, SEEK_SET, 0);

    status = null_hunter(fp_3GB, &stats);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, (unsigned)3 * 1024 * 1024 * 1024);
    CU_ASSERT_EQUAL(stats.total_null_segments, 1);
    CU_ASSERT_EQUAL(stats.longest_segment_size, (unsigned)3 * 1024 * 1024 * 1024);
    CU_ASSERT_EQUAL(stats.longest_last_segment_offset, 0);
}

void test_3GB_file_two_segments() {
    NULL_STATS stats = {0};
    NH_STATUS status = NH_SUCCESS;

    fseek(fp_3GB, 99, SEEK_SET);
    fputc('A', fp_3GB);
    fseek(fp_3GB, SEEK_SET, 0);

    status = null_hunter(fp_3GB, &stats);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, ((unsigned)3 * 1024 * 1024 * 1024) - 1);
    CU_ASSERT_EQUAL(stats.total_null_segments, 2);
    CU_ASSERT_EQUAL(stats.longest_segment_size, ((unsigned)3 * 1024 * 1024 * 1024) - 100);
    CU_ASSERT_EQUAL(stats.longest_last_segment_offset, 100);
}

void test_4GB_file_all_zeroes() {
    NULL_STATS stats = {0};
    NH_STATUS status = NH_SUCCESS;

    fseek(fp_4GB, SEEK_SET, 0);

    status = null_hunter(fp_4GB, &stats);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, (unsigned long)4 * 1024 * 1024 * 1024);
    CU_ASSERT_EQUAL(stats.total_null_segments, 1);
    CU_ASSERT_EQUAL(stats.longest_segment_size, (unsigned long)4 * 1024 * 1024 * 1024);
    CU_ASSERT_EQUAL(stats.longest_last_segment_offset, 0);
}

void test_4GB_file_two_segments() {
    NULL_STATS stats = {0};
    NH_STATUS status = NH_SUCCESS;

    fseek(fp_4GB, 199, SEEK_SET);
    fputc('A', fp_4GB);

    fseek(fp_4GB, SEEK_SET, 0);

    status = null_hunter(fp_4GB, &stats);

    CU_ASSERT(status == NH_SUCCESS);
    CU_ASSERT_EQUAL(stats.total_null_count, ((unsigned)4 * 1024 * 1024 * 1024) - 1);
    CU_ASSERT_EQUAL(stats.total_null_segments, 2);
    CU_ASSERT_EQUAL(stats.longest_segment_size, ((unsigned)4 * 1024 * 1024 * 1024) - 200);
    CU_ASSERT_EQUAL(stats.longest_last_segment_offset, 200);
}

int init_3GB_test_file() {
    char buffer[3 * 1024 * 1024] = {}; //3 MB
    fp_3GB = tmpfile();

    if(!fp_3GB) {
        printf("Failed to create 3GB test file\n");
        return ERROR;
    }

    for(int i=0; i<1024; i++) {
        int written = fwrite(buffer, sizeof(buffer), 1, fp_3GB);
        if(written != 1) {
            printf("Failed to initialize 3GB test file\n");
            return ERROR;
        }
    }

    fseek(fp_3GB, SEEK_SET, 0);

    return SUCCESS;
}

int close_3GB_test_file() {
    fclose(fp_3GB);
    return SUCCESS;
}

int init_4GB_test_file() {
    char buffer[4 * 1024 * 1024] = {}; //4 MB
    fp_4GB = tmpfile();

    if(!fp_4GB) {
        printf("Failed to create 4GB test file\n");
        return ERROR;
    }

    for(int i=0; i<1024; i++) {
        int written = fwrite(buffer, sizeof(buffer), 1, fp_4GB);
        if(written != 1) {
            printf("Failed to initialize 4GB test file\n");
            return ERROR;
        }
    }

    fseek(fp_4GB, SEEK_SET, 0);

    return SUCCESS;
}

int close_4GB_test_file() {
    fclose(fp_4GB);
    return SUCCESS;
}

int main()
{
    int num_failed_tests = 0;

    CU_basic_set_mode(CU_BRM_VERBOSE);

    CU_ErrorCode cu_status = CU_initialize_registry();
    if(cu_status != CUE_SUCCESS) {
        printf("Error setting up test registry: %s\n", CU_get_error_msg());
        return cu_status;
    }

    CU_pSuite nh_test_suite = CU_add_suite("null_hunter_test_suite", NULL, NULL);
    CU_ErrorCode cu_status_suite_1 = CU_get_error();
    const char *cu_error_msg_suite_1 = CU_get_error_msg();

    CU_pSuite nh_test_suite_3GB = CU_add_suite("null_hunter_test_suite_3GB", init_3GB_test_file, close_3GB_test_file);
    CU_ErrorCode cu_status_suite_2 = CU_get_error();
    const char *cu_error_msg_suite_2 = CU_get_error_msg();

    CU_pSuite nh_test_suite_4GB = CU_add_suite("null_hunter_test_suite_4GB", init_4GB_test_file, close_4GB_test_file);
    CU_ErrorCode cu_status_suite_3 = CU_get_error();
    const char *cu_error_msg_suite_3 = CU_get_error_msg();

    if(cu_status_suite_1 != CUE_SUCCESS ||
       cu_status_suite_2 != CUE_SUCCESS ||
       cu_status_suite_3 != CUE_SUCCESS) {
        printf("Error setting up test suite\n");
        if(cu_status_suite_1 != CUE_SUCCESS) printf("%s: %s\n", nh_test_suite->pName, cu_error_msg_suite_1);
        if(cu_status_suite_2 != CUE_SUCCESS) printf("%s: %s\n", nh_test_suite_3GB->pName, cu_error_msg_suite_2);
        if(cu_status_suite_3 != CUE_SUCCESS) printf("%s: %s\n", nh_test_suite_4GB->pName, cu_error_msg_suite_3);
        return ERROR;
    }

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

    CU_ADD_TEST(nh_test_suite_3GB, test_3GB_file_all_zeroes);
    CU_ADD_TEST(nh_test_suite_3GB, test_3GB_file_two_segments);

    CU_ADD_TEST(nh_test_suite_4GB, test_4GB_file_all_zeroes);
    CU_ADD_TEST(nh_test_suite_4GB, test_4GB_file_two_segments);

    cu_status_suite_1 = CU_basic_run_suite(nh_test_suite);
    cu_error_msg_suite_1 = CU_get_error_msg();
    num_failed_tests += CU_get_number_of_tests_failed();

    cu_status_suite_2 = CU_basic_run_suite(nh_test_suite_3GB);
    cu_error_msg_suite_2 = CU_get_error_msg();
    num_failed_tests += CU_get_number_of_tests_failed();

    cu_status_suite_3 = CU_basic_run_suite(nh_test_suite_4GB);
    cu_error_msg_suite_3 = CU_get_error_msg();
    num_failed_tests += CU_get_number_of_tests_failed();

    if(cu_status_suite_1 != CUE_SUCCESS ||
       cu_status_suite_2 != CUE_SUCCESS ||
       cu_status_suite_3 != CUE_SUCCESS) {
        printf("Error running tests\n");
        if(cu_status_suite_1 != CUE_SUCCESS) printf("%s: %s\n", nh_test_suite->pName, cu_error_msg_suite_1);
        if(cu_status_suite_2 != CUE_SUCCESS) printf("%s: %s\n", nh_test_suite_3GB->pName, cu_error_msg_suite_2);
        if(cu_status_suite_3 != CUE_SUCCESS) printf("%s: %s\n", nh_test_suite_4GB->pName, cu_error_msg_suite_3);
        return ERROR;
    }

    if(num_failed_tests)
        printf("Number of failed tests (across all test suites) = %d\n", num_failed_tests);

    CU_cleanup_registry();

    if(num_failed_tests)
        return ERROR;    // Report error

    return SUCCESS;  // Report success
}