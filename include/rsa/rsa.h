#ifndef RSA_H
#define RSA_H

#include <gmp.h>
#include "rsa/errors.h"

/**
 * RSA digital signature scheme.
 *
 * Implements RSA key generation, signing and verification as specified
 * in FIPS 186-5 §5 and RFC 8017. Key generation follows criteria in
 * FIPS 186-5 §A.1.1 using probable primes (§A.1.3).
 *
 * All randomness is drawn from an approved DRBG (SP 800-90A).
 * Minimum supported modulus length is 2048 bits per FIPS 186-5 §5.1.
 */

typedef struct {
    mpz_t n;        /**< Modulus                                           */
    mpz_t e;        /**< Public exponent, 2^16 < e < 2^256 (FIPS §A.1.1)   */
} rsa_public_key_t;

typedef struct {
    mpz_t n;        /**< Modulus                                            */
    mpz_t d;        /**< Private exponent, d = e^-1 mod λ(n)                */
    mpz_t p;        /**< First prime factor of n                            */
    mpz_t q;        /**< Second prime factor of n                           */
    mpz_t dp;       /**< CRT exponent for p: d mod (p-1)                    */
    mpz_t dq;       /**< CRT exponent for q: d mod (q-1)                    */
    mpz_t qinv;     /**< CRT coefficient: q^-1 mod p                        */
} rsa_private_key_t;

typedef struct {
    rsa_public_key_t  pub;      /**< Public key  (n, e)                     */
    rsa_private_key_t priv;     /**< Private key (n, d, p, q, dp, dq, qinv) */
    int               nlen;     /**< Modulus bit length                     */
} rsa_keypair_t;

/**
 * Generate an RSA key pair per FIPS 186-5 §A.1.1
 *
 * Returns RSA_OK on success.
 * Returns RSA_ERR_INVALID_KEYLEN if nlen is not FIPS-approved.
 * Returns RSA_ERR_KEYGEN if generation fails after max retries.
 * Returns RSA_ERR_NOMEM on allocation failure.
 */
rsa_result_t rsa_keypair_generate(rsa_keypair_t *keypair,
                                  int nlen,
                                  gmp_randstate_t state);

/**
 * Release all memory held by a key pair
 * Zeroes all private key material before freeing.
 */
void rsa_keypair_free(rsa_keypair_t *keypair);

#endif /* RSA_H */