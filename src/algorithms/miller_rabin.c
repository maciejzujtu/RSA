#include "algorithms/miller_rabin.h"

int miller_rabin_min_rounds(int nlen) 
{
    switch (nlen) {
        case 2048: return RSA_MR_ROUNDS_2048;
        case 3072: return RSA_MR_ROUNDS_3072;
        case 4096: return RSA_MR_ROUNDS_4096;
        default:   return RSA_MR_ROUNDS_2048;
    }
}

/**
 * Miller-Rabin's probabilistic primality test 
 * which bases off of equation that states
 *
 *      n - 1 = 2^s * d     (where d is odd)
 * 
 * Now for a random natural number a ∈ [2, n-2]
 * we can factorize the equation of Fermat's theorem
 *
 * a^(n-1)            ≡ 1 mod n        <==> 
 * a^(2^s * d) - 1    ≡ 0 mod n        <==>
 *          ...                        <==>
 * (a^(2^((s-1) * d)+ 1) (a^2((s-2) * d) + 1) ... 
 * ... (a^d + 1)(a^d - 1) ≡ 0 mod n
 * 
 * That theorem allows us to predict whether a given
 * integer is a prime 
 */