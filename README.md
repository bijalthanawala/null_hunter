# Count number of NULL characters in a file

## Build
```
make
```

## Cleanup all built artifacts
```
make clean
```

## Run
```
./null_hunter <name of the file to count null-s in>
```

# Unit testing

## Install unit test package(s)

### On Debian flavour distros
```
sudo apt install libcunit1 libcunit1-dev libcunit1-doc
```

## Build unit tests
```
make test_null_hunter
```

## Run unit tests
```
./test_null_hunter
```