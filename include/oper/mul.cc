#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Mul : public Oper< T >
{
	Tensor< T > *a, *b;
	Mul( Tensor< T > *a, Tensor< T > *b ) : a( a ), b( b )
	{
		if ( a->shape.dim != 2 || b->shape.dim != 2 )
			throw std::runtime_error( "Mul : has not implemented yet" );
		if ( a->shape.dimsSize[1] != b->shape.dimsSize[0] )
			throw std::runtime_error( "Mul : shape not match" );
		this->ans = new Tensor< T >( { a->shape.dimsSize[0], b->shape.dimsSize[1] }, nullptr, a->needGrad || b->needGrad, this );
	}

	void exec( bool )
	{
		for ( uint i = 0; i < ans->shape.dimsSize[0]; ++i )
			for ( uint j = 0; j < ans->shape.dimsSize[1]; ++j )
				for ( uint k = 0; k < a->shape.dimsSize[1]; ++k )
					ans->data( i, j ) += a->data( i, k ) * b->data( k, j )
	}

	void calGrad()
	{
		if ( a->needGrad )
			for ( uint i = 0; i < a->shape.dimsSize[0]; ++i )
				for ( uint j = 0; j < a->shape.dimsSize[1]; ++j )
					for ( uint k = 0; k < ans->shape.dimsSize[1]; ++k )
						a->gradHolder->data( i, j ) += ans->gradHolder->data( i, k ) * b->data( j, k );
		if ( b->needGrad )
			for ( uint i = 0; i < b->shape.dimsSize[0]; ++i )
				for ( uint j = 0; j < b->shape.dimsSize[1]; ++j )
					for ( uint k = 0; k < ans->shape.dimsSize[0]; ++k )
						b->gradHolder->data( i, j ) += ans->gradHolder->data( k, j ) * a->data( k, i );
	}
	void clearGrad()
	{
		if ( a->needGrad )
			a->clearGrad();
		if ( b->needGrad )
			b->clearGrad();
	}
	~Mul() {}
};