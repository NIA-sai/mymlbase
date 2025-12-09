#pragma once
#include "../base.hpp"

#include <iostream>
#include <string>
template < class T >
T sto( const std::string &s )
{
	std::cerr << "Warning:use default sto function:" << s << "[" << typeid( T ).name() << "]\n";
	return T();
}

template <>
char sto( const std::string &s )
{
	return s[0];
}


template <>
int sto( const std::string &s )
{
	return std::stoi( s );
}

template <>
double sto( const std::string &s )
{
	return std::stod( s );
}

template <>
ldouble sto( const std::string &s )
{
	return std::stold( s );
}

template <>
float sto( const std::string &s )
{
	return std::stof( s );
}

template <>

long sto( const std::string &s )
{
	return std::stol( s );
}

template <>

ll sto( const std::string &s )
{
	return std::stoll( s );
}

template <>
ull sto( const std::string &s )
{
	return std::stoull( s );
}

template <>
bool sto( const std::string &s )
{
	return std::stoi( s ) != 0;
}

template <>
std::string sto( const std::string &s )
{
	return s;
}

template <>
uint sto( const std::string &s )
{
	return std::stoul( s );
}
// template < class T >
// using sto = sto_func_map< T >::sto;