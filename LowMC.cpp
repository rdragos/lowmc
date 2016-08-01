#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>

#include "LowMC.h"


/////////////////////////////
//     LowMC functions     //
/////////////////////////////


template<size_t bitsetsize>
string write(bitset<bitsetsize> in) {
    string str = "";
    for (int i = 0; i < bitsetsize; ++i) {
        if (in[i] == 0) {
            str += "0";
        } else {
            str += "1";
        }
    }
    return str;
}
block LowMC::encrypt (const block& message) {
    block c = message ^ roundkeys[0];
    for (unsigned r = 1; r <= rounds; ++r) {
        c =  Substitution(c);
        c =  MultiplyWithGF2Matrix(LinMatrices[r-1], c, r-1);
        c ^= roundconstants[r-1];
        c ^= roundkeys[r];
    }
    return c;
}


block LowMC::decrypt (const block& message) {
    block c = message;
    for (unsigned r = rounds; r > 0; --r) {
        c ^= roundkeys[r];
        c ^= roundconstants[r-1];
        c =  MultiplyWithGF2Matrix(invLinMatrices[r-1], c, r);
        c =  invSubstitution(c);
    }
    c ^= roundkeys[0];
    return c;
}


void LowMC::set_key (keyblock k) {
    key = k;
    keyschedule();
}


/////////////////////////////
// LowMC private functions //
/////////////////////////////


block LowMC::Substitution (const block& message) {
    // cout << "big message: \n" << write<blocksize>(message) << "\n";
    block temp = 0;
    //Get the identity part of the message
    temp ^= (message >> 3*numofboxes);
    //Get the rest through the Sboxes
    for (unsigned i = 1; i <= numofboxes; ++i) {
        temp <<= 3;

        temp ^= Sbox[((message >> 3*(numofboxes-i))
                      & block(0x7)).to_ulong()];

    }
    return temp;
}


block LowMC::invSubstitution (const block& message) {

    block temp = 0;
    //Get the identity part of the message
    temp ^= (message >> 3*numofboxes);
    //Get the rest through the invSboxes
    for (unsigned i = 1; i <= numofboxes; ++i) {
        temp <<= 3;
        temp ^= invSbox[ ((message >> 3*(numofboxes-i))
                         & block(0x7)).to_ulong()];
    }
    return temp;
}


block LowMC::ClassicMul(const vector<block>& matrix, const block& message) {
    block temp = 0;
    for (unsigned i = 0; i < blocksize; ++i) {
        temp[i] = (message & matrix[i]).count() & 1;
    }
    return temp;
}

int LowMC::ReadBits(const vector<block>& matrix, int x, int y) {
    int res = 0;
    for (int i = 0; i < kBlock; ++i) {
        res += matrix[x][y + i] * _two_powers[i];
    }
    return res;
}
block LowMC::GrayMul(const vector<block>& matrix, const block& message, int r) {

    bitset<blocksize> res;

    int ncaches = blocksize / cache_size;

    for (int start = 0; start < ncaches; ++start) {
        for (int chunk = 0; chunk < nchunks; chunk += kTables) {
            int row_block = start * cache_size;
            int col_block = chunk * kBlock;


            vector<vector<int>> bits(kTables, vector<int>(changes.size() + 1, 0));
            for (int t = 0; t < kTables; ++t) {
                for (int j = 0; j < changes.size(); ++j) {
                    bits[t][j+1] = bits[t][j] ^ message[(chunk+t) * kBlock + changes[j]];
                }
            }

            for (int t = 0; t < kTables; ++t) {
                for (int s = 0; s < cache_size; ++s) {
                    int further = row_block + s;
                    int power = lpowers[r][chunk+t][further];
                    res[further] = res[further] ^ bits[t][gray.getId(power)];
                }
            }
        }
    }
    return res;
}
block LowMC::MultiplyWithGF2Matrix
        (const std::vector<block>& matrix, const block& message, int r) {
    block tmp;
        if (kUseGray) {
        tmp = GrayMul(matrix, message, r);
    } else {
        tmp = ClassicMul(matrix, message);
    }
    return tmp;
}


block LowMC::MultiplyWithGF2Matrix_Key
        (const std::vector<keyblock>& matrix, const keyblock& k) {
    block temp = 0;
    for (unsigned i = 0; i < blocksize; ++i) {
        temp[i] = (k & matrix[i]).count() & 1;
    }
    return temp;
}

void LowMC::keyschedule () {
    roundkeys.clear();
    ofstream cout;
    cout.open("matrices.out", ofstream::out | ofstream::app);
    for (unsigned r = 0; r <= rounds; ++r) {
        roundkeys.push_back( MultiplyWithGF2Matrix_Key (KeyMatrices[r], key) );
        for (int i = 0; i < blocksize; ++i) {
            cout << roundkeys.back()[i];
        }
        cout << "\n";
    }
    return;
}


void LowMC::instantiate_LowMC () {
    // Create LinMatrices and invLinMatrices
    LinMatrices.clear();
    invLinMatrices.clear();

    ofstream cout("matrices.out");
    cout << "Rounds\n" << rounds << "\n";
    cout << "Blocksize\n" << blocksize << "\n";
    cout << "LinMatrices[rounds], Roundconstants[rounds], KeyMatrices[rounds+1] \n";

    for (unsigned r = 0; r < rounds; ++r) {
        // Create matrix
        std::vector<block> mat;
        // Fill matrix with random bits
        do {
            mat.clear();
            for (unsigned i = 0; i < blocksize; ++i) {
                mat.push_back( getrandblock () );
            }
        // Repeat if matrix is not invertible
        } while ( rank_of_Matrix(mat) != blocksize );
        LinMatrices.push_back(mat);

        for (int i = 0; i < blocksize; ++i) {
            for (int j = 0; j < blocksize; ++j) {
                cout << mat[i][j];
            }
            cout << "\n";
        }

        vector<vector<int>> tmp(blocksize/kBlock, vector<int>(blocksize, 0));
        for (int i = 0; i < blocksize; ++i) {
            for (int chunk = 0; chunk < blocksize/kBlock; ++chunk) {
                int conf = 0;
                for (int j = 0; j < kBlock; ++j) {
                    conf += (mat[i][chunk * kBlock + j]) * (1 << j);
                }
                tmp[chunk][i] = conf;
            }
        }
        lpowers.push_back(tmp);
        invLinMatrices.push_back(invert_Matrix (LinMatrices.back()));
    }

    // Create roundconstants
    roundconstants.clear();
    for (unsigned r = 0; r < rounds; ++r) {
        roundconstants.push_back( getrandblock () );
    }

    for (auto el: roundconstants) {
        for (int i = 0; i < blocksize; ++i) {
            cout << el[i];
        }
        cout << "\n";
    }

    // Create KeyMatrices
    KeyMatrices.clear();
    for (unsigned r = 0; r <= rounds; ++r) {
        // Create matrix
        std::vector<keyblock> mat;
        // Fill matrix with random bits
        do {
            mat.clear();
            for (unsigned i = 0; i < blocksize; ++i) {
                mat.push_back( getrandkeyblock () );
            }
        // Repeat if matrix is not of maximal rank
        } while ( rank_of_Matrix_Key(mat) < std::min(blocksize, keysize) );
        KeyMatrices.push_back(mat);

        for (int i = 0; i < blocksize; ++i) {
            for (int j = 0; j < blocksize; ++j) {
                cout << mat[i][j];
            }
            cout << "\n";
        }
    }
    cout.close();
    return;
}


/////////////////////////////
// Binary matrix functions //
/////////////////////////////


unsigned LowMC::rank_of_Matrix (const std::vector<block> matrix) {
    std::vector<block> mat; //Copy of the matrix 
    for (auto u : matrix) {
        mat.push_back(u);
    }
    unsigned size = mat[0].size();
    //Transform to upper triangular matrix
    unsigned row = 0;
    for (unsigned col = 1; col <= size; ++col) {
        if ( !mat[row][size-col] ) {
            unsigned r = row;
            while (r < mat.size() && !mat[r][size-col]) {
                ++r;
            }
            if (r >= mat.size()) {
                continue;
            } else {
                auto temp = mat[row];
                mat[row] = mat[r];
                mat[r] = temp;
            }
        }
        for (unsigned i = row+1; i < mat.size(); ++i) {
            if ( mat[i][size-col] ) mat[i] ^= mat[row];
        }
        ++row;
        if (row == size) break;
    }
    return row;
}


unsigned LowMC::rank_of_Matrix_Key (const std::vector<keyblock> matrix) {
    std::vector<keyblock> mat; //Copy of the matrix 
    for (auto u : matrix) {
        mat.push_back(u);
    }
    unsigned size = mat[0].size();
    //Transform to upper triangular matrix
    unsigned row = 0;
    for (unsigned col = 1; col <= size; ++col) {
        if ( !mat[row][size-col] ) {
            unsigned r = row;
            while (r < mat.size() && !mat[r][size-col]) {
                ++r;
            }
            if (r >= mat.size()) {
                continue;
            } else {
                auto temp = mat[row];
                mat[row] = mat[r];
                mat[r] = temp;
            }
        }
        for (unsigned i = row+1; i < mat.size(); ++i) {
            if ( mat[i][size-col] ) mat[i] ^= mat[row];
        }
        ++row;
        if (row == size) break;
    }
    return row;
}


std::vector<block> LowMC::invert_Matrix (const std::vector<block> matrix) {
    std::vector<block> mat; //Copy of the matrix 
    for (auto u : matrix) {
        mat.push_back(u);
    }
    std::vector<block> invmat(blocksize, 0); //To hold the inverted matrix
    for (unsigned i = 0; i < blocksize; ++i) {
        invmat[i][i] = 1;
    }

    unsigned size = mat[0].size();
    //Transform to upper triangular matrix
    unsigned row = 0;
    for (unsigned col = 0; col < size; ++col) {
        if ( !mat[row][col] ) {
            unsigned r = row+1;
            while (r < mat.size() && !mat[r][col]) {
                ++r;
            }
            if (r >= mat.size()) {
                continue;
            } else {
                auto temp = mat[row];
                mat[row] = mat[r];
                mat[r] = temp;
                temp = invmat[row];
                invmat[row] = invmat[r];
                invmat[r] = temp;
            }
        }
        for (unsigned i = row+1; i < mat.size(); ++i) {
            if ( mat[i][col] ) {
                mat[i] ^= mat[row];
                invmat[i] ^= invmat[row];
            }
        }
        ++row;
    }

    //Transform to identity matrix
    for (unsigned col = size; col > 0; --col) {
        for (unsigned r = 0; r < col-1; ++r) {
            if (mat[r][col-1]) {
                mat[r] ^= mat[col-1];
                invmat[r] ^= invmat[col-1];
            }
        }
    }

    return invmat;
}


///////////////////////
// Pseudorandom bits //
///////////////////////


block LowMC::getrandblock () {
    block tmp = 0;
    for (unsigned i = 0; i < blocksize; ++i) tmp[i] = getrandbit ();
    return tmp;
}

keyblock LowMC::getrandkeyblock () {
    keyblock tmp = 0;
    for (unsigned i = 0; i < keysize; ++i) tmp[i] = getrandbit ();
    return tmp;
}


// Uses the Grain LSFR as self-shrinking generator to create pseudorandom bits
// Is initialized with the all 1s state
// The first 160 bits are thrown away
bool LowMC::getrandbit () {
    bool tmp = 0;
    //If state has not been initialized yet
    if (state.none ()) {
        state.set (); //Initialize with all bits set
        //Throw the first 160 bits away
        for (unsigned i = 0; i < 160; ++i) {
            //Update the state
            tmp =  state[0] ^ state[13] ^ state[23]
                       ^ state[38] ^ state[51] ^ state[62];
            state >>= 1;
            state[79] = tmp;
        }
    }
    //choice records whether the first bit is 1 or 0.
    //The second bit is produced if the first bit is 1.
    bool choice = false;
    do {
        //Update the state
        tmp =  state[0] ^ state[13] ^ state[23]
                   ^ state[38] ^ state[51] ^ state[62];
        state >>= 1;
        state[79] = tmp;
        choice = tmp;
        tmp =  state[0] ^ state[13] ^ state[23]
                   ^ state[38] ^ state[51] ^ state[62];
        state >>= 1;
        state[79] = tmp;
    } while (! choice);
    return tmp;
}



