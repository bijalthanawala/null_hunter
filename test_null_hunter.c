#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "null_hunter.h"

/*
CU_InitializeFunc initialize_test() {

}

CU_CleanupFunc cleanup_tests() {

}
*/

void test_null_fp(void)
{
    NULL_STATS null_stats = {0};
    count_null(NULL, &null_stats);
    CU_ASSERT(null_stats.null_segments == 0);
    CU_ASSERT(null_stats.total_null_count == 0);
}

/*
void test_null_stats(void)
{
    NULL_STATS null_stats = {0};
    count_null(NULL, &null_stats);
    CU_ASSERT(null_stats.null_segments == 0);
    CU_ASSERT(null_stats.total_null_count == 0);
}
*/

int main()
{
    CU_initialize_registry();
    //CU_pSuite suite = CU_add_suite("null_hunter_test_suite", initialize_test, cleanup_tests);
    CU_pSuite suite = CU_add_suite("null_hunter_test_suite", NULL, NULL);
    CU_add_test(suite, "Test NULL file pointer", test_null_fp);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}