#include "LowMC.h"
#include <iostream>

#include <ctime>
#include <sys/time.h>
#include <cstdlib>

using namespace std;
//////////////////
//     MAIN     //
//////////////////
long long timeval_diff(struct timeval *start_time, struct timeval *end_time)
{ struct timeval temp_diff;
  struct timeval *difference;
  difference=&temp_diff;
  difference->tv_sec =end_time->tv_sec -start_time->tv_sec ;
  difference->tv_usec=end_time->tv_usec-start_time->tv_usec;
  while(difference->tv_usec<0)
  { difference->tv_usec+=1000000;
    difference->tv_sec -=1;
  }
  return 1000000LL*difference->tv_sec+difference->tv_usec;
}

block get_random_block() {
    block res;
    for (int i = 0; i < blocksize; ++i) {
        res[i] = rand() % 2;
    }
    return res;
}
int main (int argc, char *argv[]) {
    // Example usage of the LowMC class
    // Instantiate a LowMC cipher instance called cipher using the key '1'.
    srand(time(0));
    if (argc != 6) {
        cout << "Incorrect number of args!!!";
        return 0;
    }
    int ITER = atoi(argv[1]);
    int FLAG = atoi(argv[2]);
    int CACHE_SIZE = atoi(argv[3]);
    int KBLOCK = atoi(argv[4]);
    int TABLES = atoi(argv[5]);

    LowMC cipher1(1, FLAG, CACHE_SIZE, KBLOCK, TABLES);
    LowMC cipher2(1, !FLAG, CACHE_SIZE, KBLOCK, TABLES);
    vector<block> plains(ITER);
    for (int iter = 0; iter < ITER; ++iter) {
        // plains[iter] = get_random_block();
        plains[iter] = iter;
    }
    timeval totalstartv, totalendv;
    gettimeofday(&totalstartv, NULL);
    for (int iter = 0; iter < ITER; ++iter) {
        block m1 = cipher1.encrypt(plains[iter]);
        // cout << (m1 == cipher2.encrypt(plains[iter]));
    }

    gettimeofday(&totalendv, NULL);
    double elapsed = timeval_diff(&totalstartv, &totalendv);
    cout << "Time2 = " << elapsed/1000000 << " seconds " << endl << flush; 
    return 0;
}
