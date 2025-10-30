CC := gcc
CFLAGS := -Wall -Wpedantic -std=c2x
BUILD_DIR := build
TEST_DIR := test
COVERAGE_DIR := coverage

all: $(BUILD_DIR) null_hunter build_tests

run_tests: build_tests
	$(TEST_DIR)/test_null_hunter

run_tests_with_coverage: build_tests_with_coverage
	$(COVERAGE_DIR)/test_null_hunter_coverage
	gcov --object-directory $(COVERAGE_DIR) --no-output --function-summaries test_null_hunter_coverage-null_hunter
	gcov --object-directory $(COVERAGE_DIR) --stdout test_null_hunter_coverage-null_hunter > $(COVERAGE_DIR)/null_hunter.c.gcov

check: run_tests

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(COVERAGE_DIR):
	mkdir -p $(COVERAGE_DIR)

null_hunter: $(BUILD_DIR)/null_hunter.o $(BUILD_DIR)/main.o
	$(CC) -o null_hunter $(BUILD_DIR)/null_hunter.o $(BUILD_DIR)/main.o

build_tests: $(BUILD_DIR)/null_hunter.o $(TEST_DIR)/test_null_hunter.o
	$(CC) -o $(TEST_DIR)/test_null_hunter $(BUILD_DIR)/null_hunter.o $(TEST_DIR)/test_null_hunter.o -lcunit

build_tests_with_coverage: $(COVERAGE_DIR)
	$(CC) -O0 --coverage -o $(COVERAGE_DIR)/test_null_hunter_coverage null_hunter.c $(TEST_DIR)/test_null_hunter.c -lcunit

$(BUILD_DIR)/null_hunter.o: null_hunter.c null_hunter.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/null_hunter.o null_hunter.c

$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/main.o main.c

$(TEST_DIR)/test_null_hunter.o: $(TEST_DIR)/test_null_hunter.c null_hunter.h
	$(CC) $(CFLAGS) -c -o $(TEST_DIR)/test_null_hunter.o $(TEST_DIR)/test_null_hunter.c

clean:
	find . -type f -name null_hunter -delete
	find . -type f -name test_null_hunter -delete
	find . -type f -name test_null_hunter_coverage -delete
	find . -name "*.o" -delete
	find . -name "*.gcno" -delete
	find . -name "*.gcda" -delete
	find . -name "*.gcov" -delete
