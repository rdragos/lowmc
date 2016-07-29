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

int main (int argc, char *argv[]) {
    // Example usage of the LowMC class
    // Instantiate a LowMC cipher instance called cipher using the key '1'.

    if (argc != 3) {
        cout << "Incorrect number of args!!!";
        return 0;
    }
    int ITER = atoi(argv[1]);
    int HOW_MANY = atoi(argv[2]);

    LowMC cipher(1);
    
    timeval totalstartv, totalendv;
    gettimeofday(&totalstartv, NULL);

    for (int iter = 0; iter < ITER; ++iter) {
        block m = iter;
        std::cout << "Plaintext:" << std::endl;
        std::cout << m << std::endl;
        m = cipher.encrypt( m );
        std::cout << "Ciphertext:" << std::endl;
        std::cout << m << std::endl;
    }

    gettimeofday(&totalendv, NULL);
    double elapsed = timeval_diff(&totalstartv, &totalendv);
    cout << "Time2 = " << elapsed/1000000 << " seconds " << endl << flush; 
    return 0;
}
