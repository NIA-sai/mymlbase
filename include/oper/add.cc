#pragma once
#include "../tensor/tensor.cc"
#include "oper.hpp"
template < typename T >
struct Mul : public Oper
{
	Tensor< T > *ans, *a, *b;
	Mul( Tensor< T > *a, Tensor< T > *b ) : a( a ), b( b )
	{
		if ( a->shape != b->shape )
			throw std::runtime_error( "Add : shape not match" );
		this->ans = new Tensor< T >( a->shape, nullptr, a->needGrad || b->needGrad, this );
	}

	void exec( bool )
	{
		if ( a->shape.dim == 0 )
			ans->data( 0 ) = a->data( 0 ) + b->data( 0 );
		else if ( a->shape.dim == 1 )
			for ( uint i = 0; i < ans->shape.dimsSize[0]; ++i )
				ans->data( i ) = a->data( i ) + b->data( i );
		else if ( a->shape.dim == 2 )
			for ( uint i = 0; i < ans->shape.dimsSize[0]; ++i )
				for ( uint j = 0; j < ans->shape.dimsSize[1]; ++j )
					ans->data( i, j ) = a->data( i, j ) + b->data( i, j );
		else
		{
			throw std::runtime_error( "Add : has not implemented yet" );
		}
	}

	void calGrad()
	{
		if ( ans->shape.dim == 0 )
		{
			if ( a->needGrad )
				a->grad->data( 0 ) += ans->grad->data( 0 );
			if ( b->needGrad )
				b->grad->data( 0 ) += ans->grad->data( 0 );
		}
		else if ( ans->shape.dim == 1 )
		{
			for ( uint i = 0; i < ans->shape.dimsSize[0]; ++i )
			{
				if ( a->needGrad )
					a->grad->data( i ) += ans->grad->data( i );
				if ( b->needGrad )
					b->grad->data( i ) += ans->grad->data( i );
			}
		}
		else if ( ans->shape.dim == 2 )
		{
			for ( uint i = 0; i < ans->shape.dimsSize[0]; ++i )
				for ( uint j = 0; j < ans->shape.dimsSize[1]; ++j )
				{
					if ( a->needGrad )
						a->grad->data( i, j ) += ans->grad->data( i, j );
					if ( b->needGrad )
						b->grad->data( i, j ) += ans->grad->data( i, j );
				}
		}
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