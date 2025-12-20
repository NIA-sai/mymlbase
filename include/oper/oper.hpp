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
	virtual void buildGrad( TensorHolder< T > & ) = 0;
	virtual void clearGrad() = 0;
	virtual void reset() = 0;
	// virtual void backward() = 0;
	virtual ~Oper() {};
};
#include "add.cc"
#include "add_scaler.cc"
#include "e_mul.cc"
#include "f/ReLU.cc"
#include "f/sigmoid.cc"
#include "mul2d.cc"
#include "mul_scaler.cc"
#include "n_pow.cc"
#include "scaler_sub.cc"
#include "sub.cc"
#include "sum.cc"
#include "transpose.cc"
