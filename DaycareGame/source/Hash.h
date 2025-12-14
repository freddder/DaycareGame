#pragma once

#include <string>

typedef long long Hash_v;

const Hash_v EMPTY_HASH = 0;

Hash_v ComputeHash(const std::string& s);