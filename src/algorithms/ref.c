#include <stddef.h>
#include "algorithms/extended_gcd.h"

/*
 * extended_gcd - recursive extended Euclidean algorithm
 *
 * Computes gcd(a, b) and Bezout coefficients x, y such that:
 *
 *     a*x + b*y = gcd(a, b)
 *
 * Base case: gcd(a, 0) = a, x = 1, y = 0
 */
gcd_result_t extended_gcd(mpz_t g, mpz_t x, mpz_t y,
                           const mpz_t a, const mpz_t b)
{
    if (mpz_sgn(a) <= 0 || mpz_sgn(b) <= 0)
        return GCD_ERR_INPUT;

    /* Base case: gcd(a, 0) = a */
    if (mpz_cmp_ui(b, 0) == 0) {
        mpz_set(g, a);
        if (x) mpz_set_ui(x, 1);
        if (y) mpz_set_ui(y, 0);
        return GCD_OK;
    }

    mpz_t g1, x1, y1, rem, quot;
    mpz_init(g1);
    mpz_init(x1);
    mpz_init(y1);
    mpz_init(rem);
    mpz_init(quot);

    /* rem = a mod b,  quot = a / b */
    mpz_tdiv_qr(quot, rem, a, b);

    /* Recurse: extended_gcd(b, a mod b) */
    gcd_result_t ret = extended_gcd(g1, x1, y1, b, rem);
    if (ret != GCD_OK)
        goto cleanup;

    /* Back-substitute Bezout coefficients:
     *   x = y1
     *   y = x1 - quot * y1  */
    mpz_set(g, g1);
    if (x) mpz_set(x, y1);
    if (y) {
        mpz_mul(y1, quot, y1);
        mpz_sub(y, x1, y1);
    }

cleanup:
    mpz_clear(g1);
    mpz_clear(x1);
    mpz_clear(y1);
    mpz_clear(rem);
    mpz_clear(quot);
    return ret;
}

/*
 * mod_inverse - compute a^-1 mod m using extended_gcd
 *
 * Finds result such that (a * result) ≡ 1 (mod m).
 * The inverse exists only if gcd(a, m) = 1.
 */
gcd_result_t mod_inverse(mpz_t result, const mpz_t a, const mpz_t m)
{
    if (mpz_cmp_ui(m, 1) <= 0)
        return GCD_ERR_MODULUS;

    mpz_t g, x, y;
    mpz_init(g);
    mpz_init(x);
    mpz_init(y);

    gcd_result_t ret = extended_gcd(g, x, y, a, m);
    if (ret != GCD_OK)
        goto cleanup;

    /* Inverse exists only if gcd(a, m) = 1 */
    if (mpz_cmp_ui(g, 1) != 0) {
        ret = GCD_ERR_NO_INVERSE;
        goto cleanup;
    }

    /* x may be negative — reduce into [0, m-1] */
    mpz_mod(result, x, m);

cleanup:
    mpz_clear(g);
    mpz_clear(x);
    mpz_clear(y);
    return ret;
}

/*
 * gcd - thin wrapper returning only gcd(a, b)
 */
void gcd(mpz_t result, const mpz_t a, const mpz_t b)
{
    mpz_gcd(result, a, b);
}