#ifndef RSA_MOD_EXP_H
#define RSA_MOD_EXP_H

#include <gmp.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file miller_rabin.h
 * @brief FIPS 186-5 §B.3.1 
 *  Miller-Rabin probabilistic primality test.
 *
 * 
 * 
 * 
 * Implementation of Miller-Rabin's primality test as defined
 * in FIPS 186-5 Appendix B.3.1. 
 * approved DRBG (SP 800-90A).
 *
 * Error probability per call: <= 4^(-rounds).
 * FIPS minimum rounds: 5 for 2048-bit, 4 for 3072-bit and above.
 */



#ifdef __cplusplus
}
#endif




#endif