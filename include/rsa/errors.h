#ifndef RSA_ERRORS_H
#define RSA_ERRORS_H

/**
 * RSA library return codes.
 *
 * All public API functions return rsa_result_t. 
 * Negative values indicate failure. 
 * RSA_OK is the only success value.
 */
typedef enum {
    RSA_OK                  =  0,   /**< Operation completed successfully.          */
    RSA_ERR_INVALID_KEYLEN  = -1,   /**< nlen is not a FIPS-approved modulus size.  */
    RSA_ERR_KEYGEN          = -2,   /**< Key generation failed after max retries.   */
    RSA_ERR_INVALID_MSG     = -3,   /**< Message out of range [1, n-1].             */
    RSA_ERR_INVALID_SIG     = -4,   /**< Signature out of range [1, n-1].           */
    RSA_ERR_NULL_KEY        = -5,   /**< NULL or uninitialised key passed.          */
    RSA_ERR_WEAK_KEY        = -6,   /**< Key fails FIPS 186-5 §A.1.1 constraints.  */
    RSA_ERR_BAD_PADDING     = -7,   /**< PKCS#1 v1.5 padding validation failed.    */
    RSA_ERR_NOMEM           = -8,   /**< Memory allocation failed.                 */
} rsa_result_t;

/**
 * Return description of an error code as a pointer
 * to a static string describijng the error.
 */
const char *rsa_strerror(rsa_result_t err);


#endif /* RSA_ERRORS_H */