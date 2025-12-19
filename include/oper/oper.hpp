#pragma once
#include "../base.hpp"
#include "../tensor.hpp"
template < class T >
struct Oper
{
	static const uint paramCnt = 2;
	// only one result!!!
	// static const uint ansCnt = 1;
	virtual void exec( TensorHolder< T > & ) = 0;
	virtual void buildGrad( const TensorHolder< T > & ) = 0;
	virtual void clearGrad() = 0;
	// virtual void backward() = 0;
	virtual ~Oper() {};
};
#include "add.cc"
#include "mul2d.cc"
#include "sub.cc"
#include "transpose.cc"
