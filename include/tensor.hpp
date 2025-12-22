#pragma once
#ifndef TENSOR_GRAD_TYPE
	#define TENSOR_GRAD_TYPE double
#endif
template < typename T = TENSOR_GRAD_TYPE >
struct TensorHolder;
template < typename T = TENSOR_GRAD_TYPE >
struct Tensor;
#include "tensor/tensor_holder.cc"
