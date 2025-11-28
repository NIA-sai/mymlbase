#pragma once
#define 𤳳 Tensor
#include "arr.cc"
typedef unsigned int uint;
typedef unsigned long long ull;
struct TensorShape
{
	uint dim = 0;
	ull size = 1;
	ull offset = 0;
	uint *dimsSize = nullptr;
	ull *stride = nullptr;
	// template < ull N = 1 >
	// TensorShape( const uint ( &dimsSize )[N] )
	// {
	// 	this->dimsSize = new uint[N];
	// 	for ( ull i = 0; i < N; ++i )
	// 		this->dimsSize[i] = dimsSize[i];
	// }
	TensorShape( uint dim, const uint *dimsSize, uint dimsSizeSize = 0 )
	{
		if ( dim == 0 )
		{
			this->dimsSize = new uint[1];
			this->dimsSize[0] = 0;
		}
		else if ( dim < dimsSizeSize )
			throw std::runtime_error( "Tensor Shape: dimsSize more than dim " );
		else
		{
			this->dim = dim;
			this->dimsSize = new uint[dim];
			this->stride = new ull[dim];
			dimsSizeSize = ( dimsSizeSize ? dimsSizeSize : dim ) - 1;
			this->stride[dim - 1] = 1;
			for ( uint i = dim - 1; i > 0; --i )
			{
				this->dimsSize[i] = i > dimsSizeSize ? 1 : dimsSize[i];
				this->stride[i - 1] = this->stride[i] * this->dimsSize[i];
			}
			this->dimsSize[0] = dimsSize[0];
			this->size = this->stride[0] * dimsSize[0];
		}
	}
	TensorShape( const TensorShape &t ) : dim( t.dim ), size( t.size ), offset( t.offset )
	{
		if ( dim )
		{
			this->stride = new ull[dim];
			this->dimsSize = new uint[dim];
			for ( uint i = 0; i < dim; ++i )
			{
				this->dimsSize[i] = t.dimsSize[i];
				this->stride[i] = t.stride[i];
			}
		}
		else
		{
			this->dimsSize = new uint[1];
			this->dimsSize[0] = 0;
		}
	}
	TensorShape( TensorShape &&t ) : dim( t.dim ), size( t.size ), offset( t.offset ), dimsSize( t.dimsSize ), stride( t.stride )
	{
		t.dimsSize = nullptr;
		t.stride = nullptr;
	}
	TensorShape &operator=( const TensorShape &t )
	{
		if ( this != &t )
		{
			delete[] this->dimsSize;
			delete[] this->stride;
			this->dim = t.dim;
			if ( dim )
			{
				this->size = t.size;
				this->offset = t.offset;
				this->dimsSize = new uint[t.dim];
				this->stride = new ull[t.dim];
				for ( uint i = 0; i < t.dim; ++i )
				{
					this->dimsSize[i] = t.dimsSize[i];
					this->stride[i] = t.stride[i];
				}
			}
			else
			{
				this->dimsSize = new uint[1];
				this->dimsSize[0] = 0;
			}
		}
		return *this;
	}
	TensorShape &operator=( TensorShape &&t )
	{
		if ( this != &t )
		{
			delete[] this->dimsSize;
			delete[] this->stride;
			this->dim = t.dim;
			this->size = t.size;
			this->offset = t.offset;
			this->dimsSize = t.dimsSize;
			this->stride = t.stride;
			t.dimsSize = nullptr;
			t.stride = nullptr;
		}
		return *this;
	}
	~TensorShape()
	{
		delete[] dimsSize;
		delete[] stride;
	}
};

template < typename T >
struct Tensor : public Arr< T >
{
	TensorShape shape;
	uint currentDim = 0;
	template < ull N = 1 >
	Tensor( uint n, const uint ( &dimsSize )[N] = { 1 } ) : shape( n, dimsSize, N ), currentDim( n ), Arr< T >( shape.size ) {}
	Tensor( TensorShape &&shape, const uint currentDim ) : shape( std::move( shape ) ), currentDim( currentDim ) {}
	// Tensor( const TensorShape &shape,const uint  ) : shape( shape ) {}
	Tensor( const Tensor &t ) : Arr< T >( t ), shape( t.shape ), currentDim( t.currentDim ) {}
	Tensor( Tensor &&t ) : Arr< T >( std::move( t ) ), shape( std::move( t.shape ) ), currentDim( t.currentDim ) {}
	Tensor &operator=( const Tensor &t )
	{
		if ( this != &t )
		{
			Arr< T >::operator=( t );
			this->shape = t.shape;
			this->currentDim = t.currentDim;
		}
		return *this;
	}
	Tensor &operator=( Tensor &&t )
	{
		if ( this != &t )
		{
			Arr< T >::operator=( std::move( t ) );
			this->shape = std::move( t.shape );
			this->currentDim = t.currentDim;
		}
		return *this;
	}
	Tensor &operator()( const uint start = 0, const uint end = 0, const uint step = 1 )
	{
		if ( currentDim == shape.dim )
			throw std::runtime_error( "Tensor: not a tensor" );
		if ( end == -1 )
			end = shape.dimsSize[currentDim] - 1;
		else if ( end == 0 )
			end = start;
		return *this;
	}
};
#include <iostream>
int main( int argc, char const *argv[] )
{
	// TensorShape< 2 > shape( { 1, 2 } );
	Tensor< int > t( 2, { 2 } );
	std::cout << t.shape.dimsSize << std::endl;
	t( 1 )( 2 );
	return 0;
}
