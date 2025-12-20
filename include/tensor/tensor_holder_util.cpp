#pragma once
#include "tensor_holder.cc"
template < typename T >
TensorHolder< T > TensorHolder< T >::sigmoid() &
{
	Sigmoid< T > *op = new Sigmoid< T >( this );
	return TensorHolder( this->hasGrad, op );
}
template < typename T >
TensorHolder< T > TensorHolder< T >::sigmoid() &&
{
	Sigmoid< T > *op = new Sigmoid< T >( new TensorHolder( std::move( *this ) ), true );
	return TensorHolder( this->hasGrad, op );
}
template < typename T >
TensorHolder< T > TensorHolder< T >::ReLU() &
{
	ReLU_OP< T > *op = new ReLU_OP< T >( this );
	return TensorHolder( this->hasGrad, op );
}
template < typename T >
TensorHolder< T > TensorHolder< T >::ReLU() &&
{
	 ReLU_OP< T > *op = new ReLU_OP< T >( new TensorHolder( std::move( *this ) ), true );
	return TensorHolder( this->hasGrad, op );
}