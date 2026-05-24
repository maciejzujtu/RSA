#ifndef RSA_EXTENDED_GCD_H
#define RSA_EXTENDED_GCD_H

#include <gmp.h>

/**
 * Extended Euclidean algorithm and modular inverse.
 *
 * Used during key generation to compute d = e^-1 mod λ(n),
 * where λ(n) = LCM(p-1, q-1) as required by FIPS 186-5 §A.1.1.
 */

typedef enum {
    GCD_OK              =  0,
    GCD_ERR_NO_INVERSE  = -1,
    GCD_ERR_MODULUS     = -2,
    GCD_ERR_INPUT       = -3,
} gcd_result_t;

/**
 * Computes gcd and Bezout coefficients x, y such
 * that a*x + b*y = gcd(a, b)
 */
gcd_result_t extended_gcd(mpz_t gcd, mpz_t x, mpz_t y,
                           const mpz_t a, const mpz_t b);

/**
 * Finds result such that (a * result) ≡ 1 (mod m).
 * Called by keygen to compute d = e^-1 mod λ(n).
 */
gcd_result_t mod_inverse(mpz_t result, const mpz_t a, const mpz_t m);

/**
 * Thin wrapper for callers that only need the gcd and not
 * the Bezout coefficients.
 */
void gcd(mpz_t result, const mpz_t a, const mpz_t b);

#endif /* RSA_EXTENDED_GCD_H */