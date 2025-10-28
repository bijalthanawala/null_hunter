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
e.g.
```
./null_hunter null_hunter
Scanning file null_hunter
Found total 3564 NULL characters in 232 segments
```

## Unit testing

### Install unit test package(s)

#### On Debian flavour distros
```
sudo apt install libcunit1 libcunit1-dev libcunit1-doc
```

### Build unit tests
```
make test_null_hunter
```

### Run unit tests
```
./test_null_hunter
```

## Cleanup all built artifacts
```
make clean
```