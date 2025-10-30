CC := gcc
CFLAGS := -Wall -Wpedantic -std=c2x
BUILD_DIR := build
TEST_DIR := test
COVERAGE_DIR := coverage

all: $(BUILD_DIR) null_hunter $(TEST_DIR)/test_null_hunter

test: $(TEST_DIR)/test_null_hunter
	$(TEST_DIR)/test_null_hunter

check: test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

null_hunter: $(BUILD_DIR)/null_hunter.o $(BUILD_DIR)/main.o
	$(CC) -o null_hunter $(BUILD_DIR)/null_hunter.o $(BUILD_DIR)/main.o

$(BUILD_DIR)/null_hunter.o: null_hunter.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/null_hunter.o null_hunter.c

$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/main.o main.c

$(TEST_DIR)/test_null_hunter: $(BUILD_DIR)/null_hunter.o $(TEST_DIR)/test_null_hunter.o
	$(CC) -o $(TEST_DIR)/test_null_hunter $(BUILD_DIR)/null_hunter.o $(TEST_DIR)/test_null_hunter.o -lcunit

$(TEST_DIR)/test_null_hunter.o: $(TEST_DIR)/test_null_hunter.c
	$(CC) $(CFLAGS) -c -o $(TEST_DIR)/test_null_hunter.o $(TEST_DIR)/test_null_hunter.c

$(COVERAGE_DIR):
	mkdir -p $(COVERAGE_DIR)

test_with_coverage: $(COVERAGE_DIR)
	$(CC) -O0 --coverage -o $(COVERAGE_DIR)/test_null_hunter_coverage null_hunter.c $(TEST_DIR)/test_null_hunter.c -lcunit
	$(COVERAGE_DIR)/test_null_hunter_coverage
	gcov $(COVERAGE_DIR)/test_null_hunter_coverage-null_hunter

clean:
	find . -name null_hunter -delete
	find . -name test_null_hunter -delete
	find . -name test_null_hunter_coverage -delete
	find . -name "*.o" -delete
	find . -name "*.gcno" -delete
	find . -name "*.gcda" -delete
	find . -name "*.gcov" -delete
