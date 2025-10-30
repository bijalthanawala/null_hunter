# Count number of NULL characters in a file

## Build & Run

### Build
```
make
```

### Run
```
./null_hunter <name of the file to count null-s in>
```
Example output:
```
 ./null_hunter /bin/passwd
Scanning file /bin/passwd
Found total 29889 NULL characters in 4755 segments, with longest segment of 3915 characters at offset 12469
```

## Unit testing

### Install unit test package(s)

#### On Debian flavour distros
```
sudo apt install libcunit1 libcunit1-dev libcunit1-doc
```

### Build & run unit tests using this single command
```
make run_tests
```
_The above command runs unit tests automatically after building_

Example output:
```
make run_tests
gcc -Wall -Wpedantic -std=c2x -c -o build/null_hunter.o null_hunter.c
gcc -Wall -Wpedantic -std=c2x -c -o test/test_null_hunter.o test/test_null_hunter.c
gcc -o test/test_null_hunter build/null_hunter.o test/test_null_hunter.o -lcunit
test/test_null_hunter


     CUnit - A unit testing framework for C - Version 2.1-3
     http://cunit.sourceforge.net/


Suite: null_hunter_file_test_suite
  Test: test_null_fp ...passed
  Test: test_null_stats ...passed
  Test: test_zero_length_file ...passed
  Test: test_no_null_file_size_one ...passed
  Test: test_no_null_file_size_five ...passed
  Test: test_one_null_file_size_one ...passed
  Test: test_nulls_in_the_beginning ...passed
  Test: test_nulls_in_the_end ...passed
  Test: test_five_null_one_segment_only_in_file ...passed
  Test: test_five_null_one_segment_followed_by_one_nonnull ...passed
  Test: test_two_nulls_two_segments_both_segments_at_each_end ...passed
  Test: test_two_segments_both_segments_in_between ...passed
  Test: test_char_0xff_in_between ...passed
  Test: test_offset_of_longest_segment ...passed
  Test: test_offset_of_last_longest_segment ...passed
  Test: test_3GB_file_all_zeroes ...passed
  Test: test_3GB_file_two_segments ...passed
  Test: test_4GB_file_all_zeroes ...passed
  Test: test_4GB_file_two_segments ...passed

Run Summary:    Type  Total    Ran Passed Failed Inactive
              suites      1      1    n/a      0        0
               tests     19     19     19      0        0
             asserts     76     76     76      0      n/a

Elapsed time =   35.454 seconds
```

### Alternatively, build & run unit tests _with coverage_ using this single command
```
make run_tests_with_coverage
```
_The above command builds, runs unit tests and shows the coverage_

Example output:
```
make run_tests_with_coverage
gcc -O0 --coverage -o coverage/test_null_hunter_coverage null_hunter.c test/test_null_hunter.c -lcunit
coverage/test_null_hunter_coverage


     CUnit - A unit testing framework for C - Version 2.1-3
     http://cunit.sourceforge.net/


Suite: null_hunter_file_test_suite
  Test: test_null_fp ...passed
  Test: test_null_stats ...passed
  Test: test_zero_length_file ...passed
  Test: test_no_null_file_size_one ...passed
  Test: test_no_null_file_size_five ...passed
  Test: test_one_null_file_size_one ...passed
  Test: test_nulls_in_the_beginning ...passed
  Test: test_nulls_in_the_end ...passed
  Test: test_five_null_one_segment_only_in_file ...passed
  Test: test_five_null_one_segment_followed_by_one_nonnull ...passed
  Test: test_two_nulls_two_segments_both_segments_at_each_end ...passed
  Test: test_two_segments_both_segments_in_between ...passed
  Test: test_char_0xff_in_between ...passed
  Test: test_offset_of_longest_segment ...passed
  Test: test_offset_of_last_longest_segment ...passed
  Test: test_3GB_file_all_zeroes ...passed
  Test: test_3GB_file_two_segments ...passed
  Test: test_4GB_file_all_zeroes ...passed
  Test: test_4GB_file_two_segments ...passed

Run Summary:    Type  Total    Ran Passed Failed Inactive
              suites      1      1    n/a      0        0
               tests     19     19     19      0        0
             asserts     76     76     76      0      n/a

Elapsed time =   42.297 seconds
gcov coverage/test_null_hunter_coverage-null_hunter
File 'null_hunter.c'
Lines executed:80.00% of 35
Creating 'null_hunter.c.gcov'

Lines executed:80.00% of 35
```


## Cleanup all built artifacts
```
make clean
```
Example output:
```
make clean
find . -name null_hunter -delete
find . -name test_null_hunter -delete
find . -name test_null_hunter_coverage -delete
find . -name "*.o" -delete
find . -name "*.gcno" -delete
find . -name "*.gcda" -delete
find . -name "*.gcov" -delete
```