#pragma once
#include <cfloat>
#include <climits>

typedef unsigned int uint;
typedef unsigned long long ull;
typedef long long ll;
typedef long double ldouble;
constexpr ll MAX_ll = LLONG_MAX;
constexpr ll MIN_ll = LLONG_MIN;
constexpr int MAX_int = INT_MAX;
constexpr int MIN_int = INT_MIN;
constexpr ull MAX_ull = ULLONG_MAX;
constexpr ull MIN_ull = 0;
constexpr uint MAX_uint = UINT_MAX;
constexpr uint MIN_uint = 0;
constexpr double MAX_double = DBL_MAX;
constexpr double MIN_double = DBL_MIN;
constexpr float MAX_float = FLT_MAX;
constexpr float MIN_float = FLT_MIN;
constexpr ldouble MAX_ldouble = LDBL_MAX;
constexpr ldouble MIN_ldouble = LDBL_MIN;

template < int... >
using VoidValue = void;
template < typename... >
using VoidType = void;

//test
#include <iostream>
