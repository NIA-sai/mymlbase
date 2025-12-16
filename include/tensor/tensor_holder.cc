#pragma once
#include "tensor.cc"
#ifndef TENSOR_GRAD_TYPE
	#define TENSOR_GRAD_TYPE double
#endif
template < typename T = double >
struct TensorHolder
{
	Tensor< T > tensor;
	TensorHolder< TENSOR_GRAD_TYPE > *grad = nullptr;
	bool needGrad = false;
	Oper *creator = nullptr;
};