# LowMC implementation
This is a C++ implementation of the LowMC block cipher family. The
parameters (block size, number of S-boxes in the substitution layer, number
of rounds, key size) are defined in `LowMC.h`. Compilation requires support
of C++11 features.

The files `LowMC.h` and `LowMC.cpp` contain the relevant code. `test.cpp`
contains a short example usage.

The file `determine_rounds.py` contains a Python script that determines
the number of rounds needed for LowMC to be secure in dependence on
the other parameters: the block size *n*, the number of S-boxes per layer *m*,
the log2 of the allowed data complexity *d*, and the key size *k*.

Example usage: `python3 determine_rounds.py 256 63 128 128`


# LowMC Benchmarking

run make;
./test Encryptions russianFlag cacheSize graySize grayTables

Using 4 russians method with a cache size of blocksize / 256 , Gray
codes of 8 bit length and 1 Gray table

./test 10000 1 256 8 1

Encrypting 10000 with classic bitsets
./test 10000 0 256 8 1
