#include <stddef.h>
#include <string.h>

#include "rsa/rsa.h"
#include "rsa/errors.h"
#include "algorithms/miller_rabin.h"
#include "algorithms/extended_gcd.h"

#define MAX_PRIME_ATTEMPTS 1000
#define RSA_PUBLIC_EXPONENT 65537

/*
 * Generate a random probable prime of `bits` bits
 *
 * Generates probable primes of `bits` bits until one of them 
 * passes Miller-Rabin and the gcd check.
 * 
 * Returns RSA_OK on success.
 * Returns RSA_ERR_KEYGEN if no prime found in `MAX_PRIME_ATTEMPTS` iterations.
 */
static rsa_result_t generate_prime(mpz_t p,
                                   int bits,
                                   const mpz_t e,
                                   gmp_randstate_t state)
{
    mpz_t p_minus_1, g;
    int rounds = miller_rabin_min_rounds(bits * 2);
    int attempts = 0;

    mpz_init(p_minus_1);
    mpz_init(g);

    while (attempts++ < MAX_PRIME_ATTEMPTS) {

        /* Generate random bits-bit candidate, force top bit set
         * so p >= 2^(bits-1) per FIPS 186-5 §A.1.1 §2(b) */
        mpz_urandomb(p, state, bits);
        mpz_setbit(p, bits - 1);

        /* Force odd — even numbers are trivially composite */
        mpz_setbit(p, 0);

        /* Quick divisibility check against small primes before
         * running the full Miller-Rabin test */
        if (mpz_divisible_ui_p(p, 3)  ||
            mpz_divisible_ui_p(p, 5)  ||
            mpz_divisible_ui_p(p, 7)  ||
            mpz_divisible_ui_p(p, 11) ||
            mpz_divisible_ui_p(p, 13))
            continue;

        /* FIPS 186-5 §B.3.1 — Miller-Rabin primality test */
        if (miller_rabin(p, rounds, state) != MR_PRIME)
            continue;

        /* FIPS 186-5 §A.1.1 §2(a) — gcd(e, p-1) must be 1 */
        mpz_sub_ui(p_minus_1, p, 1);
        gcd(g, e, p_minus_1);
        if (mpz_cmp_ui(g, 1) != 0)
            continue;

        /* Valid prime found */
        mpz_clear(p_minus_1);
        mpz_clear(g);
        return RSA_OK;
    }

    mpz_clear(p_minus_1);
    mpz_clear(g);
    return RSA_ERR_KEYGEN;
}

/*
 * check_pq_distance - enforce |p-q| > 2^(nlen/2 - 100)
 * per FIPS 186-5 §A.1.1 §2(d).
 *
 * Returns 1 if the distance is acceptable, 0 if p and q are too close.
 */
static int check_pq_distance(const mpz_t p, const mpz_t q, int nlen)
{
    mpz_t diff, threshold;
    int ok;

    mpz_init(diff);
    mpz_init(threshold);

    mpz_sub(diff, p, q);
    mpz_abs(diff, diff);

    /* threshold = 2^(nlen/2 - 100) */
    mpz_ui_pow_ui(threshold, 2, nlen / 2 - 100);

    ok = (mpz_cmp(diff, threshold) > 0);

    mpz_clear(diff);
    mpz_clear(threshold);
    return ok;
}

rsa_result_t rsa_keypair_generate(rsa_keypair_t *keypair,
                                  int nlen,
                                  gmp_randstate_t state)
{
    if (!keypair || !state)
        return RSA_ERR_NULL_KEY;

    /* FIPS 186-5 §5.1 — only 2048, 3072 and 4096 are approved */
    if (nlen != 2048 && nlen != 3072 && nlen != 4096)
        return RSA_ERR_INVALID_KEYLEN;

    int half = nlen / 2;
    rsa_result_t ret = RSA_OK;

    mpz_t e, p, q, n, lambda, d, dp, dq, qinv;
    mpz_t p1, q1, tmp;

    mpz_init(e);
    mpz_init(p);     mpz_init(q);
    mpz_init(n);     mpz_init(lambda);
    mpz_init(d);
    mpz_init(dp);    mpz_init(dq);    mpz_init(qinv);
    mpz_init(p1);    mpz_init(q1);    mpz_init(tmp);

    /* Step 1 — select e before generating primes (FIPS §A.1.1 §1) */
    mpz_set_ui(e, RSA_PUBLIC_EXPONENT);

    /* Steps 2–4 — generate prime p */
    ret = generate_prime(p, half, e, state);
    if (ret != RSA_OK)
        goto cleanup;

    /* Steps 5–6 — generate prime q, retry until |p-q| is large enough */
    for (int attempt = 0; attempt < MAX_PRIME_ATTEMPTS; attempt++) {
        ret = generate_prime(q, half, e, state);
        if (ret != RSA_OK)
            goto cleanup;

        /* FIPS 186-5 §A.1.1 §2(d) */
        if (check_pq_distance(p, q, nlen))
            break;

        if (attempt == MAX_PRIME_ATTEMPTS - 1) {
            ret = RSA_ERR_KEYGEN;
            goto cleanup;
        }
    }

    /* Step 7 — compute n = p * q */
    mpz_mul(n, p, q);

    /* Step 7 — compute λ(n) = LCM(p-1, q-1) per FIPS §A.1.1 §3(b)
     * NOTE: use LCM, NOT φ(n) = (p-1)(q-1) */
    mpz_sub_ui(p1, p, 1);
    mpz_sub_ui(q1, q, 1);
    mpz_lcm(lambda, p1, q1);

    /* Step 8 — compute d = e^-1 mod λ(n) */
    gcd_result_t gret = mod_inverse(d, e, lambda);
    if (gret != GCD_OK) {
        ret = RSA_ERR_KEYGEN;
        goto cleanup;
    }

    /* FIPS §A.1.1 §3(a) — verify d > 2^(nlen/2) */
    mpz_ui_pow_ui(tmp, 2, half);
    if (mpz_cmp(d, tmp) <= 0) {
        ret = RSA_ERR_KEYGEN;
        goto cleanup;
    }

    /* Compute CRT components */
    mpz_mod(dp,   d, p1);          /* dp   = d mod (p-1)  */
    mpz_mod(dq,   d, q1);          /* dq   = d mod (q-1)  */
    mod_inverse(qinv, q, p);       /* qinv = q^-1 mod p   */

    /* Populate keypair */
    keypair->nlen = nlen;

    mpz_init_set(keypair->pub.n,  n);
    mpz_init_set(keypair->pub.e,  e);

    mpz_init_set(keypair->priv.n,    n);
    mpz_init_set(keypair->priv.d,    d);
    mpz_init_set(keypair->priv.p,    p);
    mpz_init_set(keypair->priv.q,    q);
    mpz_init_set(keypair->priv.dp,   dp);
    mpz_init_set(keypair->priv.dq,   dq);
    mpz_init_set(keypair->priv.qinv, qinv);

cleanup:
    /* Zeroise all intermediate secret values before freeing */
    mpz_set_ui(p,  0);  mpz_set_ui(q,  0);
    mpz_set_ui(d,  0);  mpz_set_ui(dp, 0);
    mpz_set_ui(dq, 0);  mpz_set_ui(qinv, 0);
    mpz_set_ui(lambda, 0);

    mpz_clear(e);
    mpz_clear(p);      mpz_clear(q);
    mpz_clear(n);      mpz_clear(lambda);
    mpz_clear(d);
    mpz_clear(dp);     mpz_clear(dq);    mpz_clear(qinv);
    mpz_clear(p1);     mpz_clear(q1);    mpz_clear(tmp);

    return ret;
}

void rsa_keypair_free(rsa_keypair_t *keypair)
{
    if (!keypair)
        return;

    /* Zeroise private key material before clearing */
    mpz_set_ui(keypair->priv.d,    0);
    mpz_set_ui(keypair->priv.p,    0);
    mpz_set_ui(keypair->priv.q,    0);
    mpz_set_ui(keypair->priv.dp,   0);
    mpz_set_ui(keypair->priv.dq,   0);
    mpz_set_ui(keypair->priv.qinv, 0);

    mpz_clear(keypair->pub.n);
    mpz_clear(keypair->pub.e);

    mpz_clear(keypair->priv.n);
    mpz_clear(keypair->priv.d);
    mpz_clear(keypair->priv.p);
    mpz_clear(keypair->priv.q);
    mpz_clear(keypair->priv.dp);
    mpz_clear(keypair->priv.dq);
    mpz_clear(keypair->priv.qinv);

    memset(keypair, 0, sizeof(rsa_keypair_t));
}
