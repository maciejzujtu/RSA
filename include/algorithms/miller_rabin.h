#ifndef RSA_MILLER_RABIN_H
#define RSA_MILLER_RABIN_H

#include <gmp.h>

/**
 * Miller-Rabin probabilistic primality test.
 * 
 * Implements the Miller-Rabin primality test as specified in
 * FIPS 186-5 §B.3.1 Numbers are generated from an approved
 * DRBG (SP 800-90A).
 */ 

#define RSA_MR_ROUNDS_2048  5
#define RSA_MR_ROUNDS_3072  4
#define RSA_MR_ROUNDS_4096  4

typedef enum {
    MR_PRIME     =  1,  /** Probably prime */
    MR_COMPOSITE =  0,  /** Is Composite   */
    MR_ERROR     = -1   /** Less than 3    */
} mr_result_t;

mr_result_t miller_rabin(const mpz_t n, int rounds, gmp_randstate_t state);

int miller_rabin_min_rounds(int nlen);

#endif /* RSA_MILLER_RABIN_H */