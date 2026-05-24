#include "rsa/errors.h"

const char *rsa_strerror(rsa_result_t err)
{
    switch (err) {
    case RSA_OK:                 return "success";
    case RSA_ERR_INVALID_KEYLEN: return "invalid key length";
    case RSA_ERR_KEYGEN:         return "key generation failed";
    case RSA_ERR_INVALID_MSG:    return "message out of range";
    case RSA_ERR_INVALID_SIG:    return "signature out of range";
    case RSA_ERR_NULL_KEY:       return "null or uninitialised key";
    case RSA_ERR_WEAK_KEY:       return "key fails FIPS 186-5 constraints";
    case RSA_ERR_BAD_PADDING:    return "PKCS#1 v1.5 padding validation failed";
    case RSA_ERR_NOMEM:          return "memory allocation failed";
    default:                     return "unknown error";
    }
}