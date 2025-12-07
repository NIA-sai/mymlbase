#pragma once
#include "../tensor/tensor.cc"
#include "oper.hpp"
template < typename T >
struct Add : public Oper
{
	Tensor< T > *ans, *a, *b;
	Add( Tensor< T > *a, Tensor< T > *b, Tensor< T > *ans = nullptr )
	{
	}
	void exec( bool );
	void calGrad();
	~Add() {}
};