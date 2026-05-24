
#include <gmp.h>
#include "algorithms/extended_gcd.h"

/**
 * Recursive extended Euclidean algorithm that
 * computes gcd(a, b) and coefficients x and y
 *
 *      (a * x) + (m * y) = gcd(a, m)
 */
gcd_result_t extended_gcd(mpz_t gcd, mpz_t x, mpz_t y,
                           const mpz_t a, const mpz_t m)
{
    if (mpz_sgn(a) <= 0 || x == NULL || y == NULL)
        return GCD_ERR_INPUT;
    
    if (mpz_cmp_ui(m, 0) == 0) {
        mpz_set(gcd, a);            
        mpz_set_ui(x, 1);
        mpz_set_ui(y, 0);
        return GCD_OK;
    }

    mpz_t g;
    mpz_t bx;
    mpz_t by;
    mpz_t rem;
    mpz_t quot;

    mpz_init(g);
    mpz_init(bx);
    mpz_init(by);
    mpz_init(rem);
    mpz_init(quot);

    mpz_tdiv_q(quot, a, m);
    mpz_tdiv_r(rem, a, m);

    gcd_result_t egcd = extended_gcd(g, bx, by, m, rem);

    if (egcd == GCD_OK) {
        mpz_set(gcd, g);
        mpz_set(x, by);
        mpz_mul(by, quot, by);
        mpz_sub(y, bx, by);
    }

    mpz_clear(g);
    mpz_clear(bx);
    mpz_clear(by);
    mpz_clear(rem);
    mpz_clear(quot);
    return egcd;
}

/**
 * Modular inverse using extended Euclidean algorithm
 * to find modular inverse of a based on the equation
 * 
 *      (a * x) ≡ 1 mod m   =>  gcd(a, m) = 1
 */
gcd_result_t mod_inverse(mpz_t result, const mpz_t a, const mpz_t m)
{
    if (mpz_cmp_ui(m, 1) <= 0)
        return GCD_ERR_MODULUS;

    mpz_t gcd;
    mpz_t x;
    mpz_t y;

    mpz_init(gcd);
    mpz_init(x);
    mpz_init(y);

    gcd_result_t egcd = extended_gcd(gcd, x, y, a, m);

    if (egcd == GCD_OK) {
        if (mpz_cmp_ui(gcd, 1) != 0) {
            egcd = GCD_ERR_NO_INVERSE;
        } else {
            mpz_mod(result, x, m);
        }
    }

    mpz_clear(gcd);
    mpz_clear(x);
    mpz_clear(y);
    return egcd;
}