#include "Hash.h"

// https://cp-algorithms.com/string/string-hashing.html
Hash_v ComputeHash(const std::string& s)
{
    const int p = 31;
    const int m = 1e9 + 9;
    Hash_v hash_value = 0;
    Hash_v p_pow = 1;
    for (char c : s) {
        hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return hash_value;
}