#ifndef RSA_MOD_EXP_H
#define RSA_MOD_EXP_H

#include <gmp.h>

/**
 * Fast modular exponentiation algorithm.
 *
 * Computes modulus in O(log(n)) multiplications using the
 * square-and-multiply algorithm as per FIPS 186-5 §B.2 specification.
 */

typedef enum {
    ME_OK            =  0,
    ME_ERR_MODULUS   = -1,  /** Modulus <= 1 */
    ME_ERR_BASE      = -2,  /** Out of range */
    ME_ERR_EXPONENT  = -3,  /** Exponent < 0 */
} me_result_t;

me_result_t mod_exp(mpz_t result,
                    const mpz_t base,
                    const mpz_t exp,
                    const mpz_t mod);

me_result_t mod_exp_ct(mpz_t result,
                       const mpz_t base,
                       const mpz_t exp,
                       const mpz_t mod);

#endif /* RSA_MOD_EXP_H */