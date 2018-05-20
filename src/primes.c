#include "primes.h"

// Miller Rabin Implementation, from Rosetta Code
static inline size_t power(size_t a, size_t n, size_t mod) {
    size_t power = a;
    size_t result = 1;

    while (n) {
        if (n & 1)
            result = (result * power) % mod;
        power = (power * power) % mod;
        n >>= 1;
    }
    return result;
}

// n−1 = 2^s * d with d odd by factoring powers of 2 from n−1
static inline bool witness(size_t n, size_t s, size_t d, size_t a) {
    size_t x = power(a, d, n);
    size_t y = 0;

    while (s) {
        y = (x * x) % n;
        if (y == 1 && x != 1 && x != n - 1)
            return false;
        x = y;
        --s;
    }
    if (y != 1)
        return false;
    return true;
}

static inline bool is_prime(size_t n) {
    if (((!(n & 1)) && n != 2) || (n < 2) || (n % 3 == 0 && n != 3))
        return false;
    if (n <= 3)
        return true;

    size_t d = n / 2;
    size_t s = 1;
    while (!(d & 1)) {
        d /= 2;
        ++s;
    }

    if (n < 1373653){
        return witness(n, s, d, 2)
               && witness(n, s, d, 3);
    }
    if (n < 9080191){
        return witness(n, s, d, 31)
               && witness(n, s, d, 73);
    }
    if (n < 4759123141) {
        return witness(n, s, d, 2)
               && witness(n, s, d, 7)
               && witness(n, s, d, 61);
    }
    if (n < 1122004669633){
        return witness(n, s, d, 2)
               && witness(n, s, d, 13)
               && witness(n, s, d, 23)
               && witness(n, s, d, 1662803);
    }
    if (n < 2152302898747) {
        return witness(n, s, d, 2)
               && witness(n, s, d, 3)
               && witness(n, s, d, 5)
               && witness(n, s, d, 7)
               && witness(n, s, d, 11);
    }
    if (n < 3474749660383){
        return witness(n, s, d, 2)
               && witness(n, s, d, 3)
               && witness(n, s, d, 5)
               && witness(n, s, d, 7)
               && witness(n, s, d, 11)
               && witness(n, s, d, 13);
    }
    return witness(n, s, d, 2)
           && witness(n, s, d, 3)
           && witness(n, s, d, 5)
           && witness(n, s, d, 7)
           && witness(n, s, d, 11)
           && witness(n, s, d, 13)
           && witness(n, s, d, 17);
}

inline size_t next_prime(size_t n){
    while(true){
        if(is_prime(n)) return n;
    }
}