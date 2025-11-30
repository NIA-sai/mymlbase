#pragma once
#include <iostream>

#define 𤳳 Tensor
#include "arr.cc"
typedef unsigned int uint;
typedef unsigned long long ull;
struct TensorShape
{
	bool isnView = true;
	bool isnSlice = true;
	uint dim = 0;
	ull size = 0;
	ull offset = 0;
	uint *dimsSize = nullptr;
	ull *stride = nullptr;
	TensorShape() = default;
	TensorShape(
	    bool isnView,
	    uint dim,
	    ull size,
	    ull offset,
	    uint *dimsSize,
	    ull *stride ) : isnView( isnView ), dim( dim ), size( size ), offset( offset ), dimsSize( dimsSize ), stride( stride ) {}
	TensorShape( const uint *dimsSize, uint dim = 0 )
	{
		if ( dim == 0 )
		{
			this->size = 1;
			this->dimsSize = new uint[1];
			this->dimsSize[0] = 0;
		}
		else
		{
			this->dim = dim;
			this->dimsSize = new uint[dim];
			this->stride = new ull[dim];
			this->stride[dim - 1] = 1;
			for ( uint i = dim - 1; i > 0; --i )
			{
				this->dimsSize[i] = dimsSize[i];
				this->stride[i - 1] = this->stride[i] * this->dimsSize[i];
			}
			this->dimsSize[0] = dimsSize[0];
			this->size = this->stride[0] * dimsSize[0];
		}
	}
	TensorShape( const TensorShape &t ) : dim( t.dim ), size( t.size ), offset( t.offset ), isnView( t.isnView )
	{
		if ( t.isnView )
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
			this->dimsSize = t.dimsSize;
			this->stride = t.stride;
		}
	}
	TensorShape( TensorShape &&t ) : dim( t.dim ), size( t.size ), offset( t.offset ), dimsSize( t.dimsSize ), stride( t.stride ), isnView( t.isnView )
	{
		t.dimsSize = nullptr;
		t.stride = nullptr;
	}
	TensorShape &operator=( const TensorShape &t )
	{
		if ( this != &t )
		{
			this->dim = t.dim;
			this->size = t.size;
			this->offset = t.offset;
			this->isnView = t.isnView;

			if ( t.isnView )
			{
				delete[] this->dimsSize;
				delete[] this->stride;
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
				this->dimsSize = t.dimsSize;
				this->stride = t.stride;
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
			this->isnView = t.isnView;
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
		if ( isnView )
		{
			delete[] dimsSize;
			delete[] stride;
		}
	}
	friend std::ostream &operator<<( std::ostream &os, const TensorShape &t );
};

std::ostream &operator<<( std::ostream &os, const TensorShape &t )
{
	os << "{\n\tdim:" << t.dim << ",\n\tsize:" << t.size << ",\n\toffset:" << t.offset;
	if ( t.dim )
	{
		os << ",\n\tdimsSize:{";
		uint i;
		for ( i = 0; i < t.dim - 1; ++i )
			os << t.dimsSize[i] << ",";
		os << t.dimsSize[i] << "},\n\tstride:{";
		for ( i = 0; i < t.dim - 1; ++i )
			os << t.stride[i] << ",";
		os << t.stride[i] << "}";
	}
	os << "\n}";
	return os;
}




template < typename T >
struct Tensor
{
	T *data = nullptr;
	// really size
	ull size = 0;
	TensorShape shape;
	template < uint N = 2 >
	Tensor( const uint ( &dimsSize )[N] = { 3, 3 }, const T *data = nullptr ) : shape( dimsSize, N )
	{
		this->data = new T[shape.size];
		this->size = shape.size;
		if ( data )
		{
			for ( ull i = 0; i < shape.size; ++i )
				this->data[i] = data[i];
		}
	}
	// to get view
	Tensor( TensorShape &&shape, T *data, ull size ) : shape( std::move( shape ) ), data( data ), size( size )
	{
		// this->shape.isnView = false;
	}

	// copy()
	Tensor( TensorShape &&shape ) : shape( std::move( shape ) ), size( shape.size ), data( new T[shape.size] ) {}
	Tensor( const Tensor &t ) : shape( t.shape ), size( t.size )
	{
		this->shape.isnView = t.shape.isnView;
		if ( t.shape.isnView )
		{
			this->data = new T[t.size];
			for ( ull i = 0; i < t.size; ++i )
				this->data[i] = t.data[i];
		}
		else
			this->data = t.data;
	}
	Tensor( Tensor &&t ) : shape( std::move( t.shape ) ), data( t.data ), size( t.size )
	{
		this->shape.isnView = t.shape.isnView;
		t.data = nullptr;
	}
	Tensor &operator=( const Tensor &t )
	{
		if ( this != &t )
		{
			if ( this->shape.isnView )
				delete[] this->data;
			this->shape = t.shape;
			this->size = t.size;
			if ( t.shape.isnView )
			{
				this->data = new T[t.shape.size];
				for ( ull i = 0; i < t.shape.size; ++i )
					this->data[i] = t.data[i];
			}
			else
				this->data = t.data;
		}
		return *this;
	}
	Tensor &operator=( Tensor &&t )
	{
		if ( this != &t )
		{
			this->shape = std::move( t.shape );
			this->size = t.size;
			this->data = t.data;
			t.data = nullptr;
		}
		return *this;
	}
	// slice
	Tensor &operator()( const uint start, uint end = 0, uint step = 1, uint operDim = 1 )
	{
		--operDim;
		if ( this->shape.dim < operDim )
			throw std::runtime_error( "Tensor: slice operDim out of Tensor,use oneValue to get" );
		if ( end >= this->shape.dimsSize[operDim] )
			end = shape.dimsSize[operDim] - 1;
		else if ( end == 0 )
		{
			end = start;
			step = 1;
		}
		else if ( end < start )
			throw std::runtime_error( "Tensor: slice end < start" );
		if ( step == 0 )
			throw std::runtime_error( "Tensor: slice step can't be 0" );

		if ( this->shape.isnSlice )
		{
			TensorShape newShape(
			    true,
			    this->shape.dim,
			    this->shape.size / this->shape.dimsSize[operDim],
			    this->shape.offset + start * this->shape.stride[operDim],
			    new uint[this->shape.dim],
			    new ull[this->shape.dim] );
			newShape.isnSlice = false;
			for ( uint i = 0; i < this->shape.dim; ++i )
			{
				newShape.dimsSize[i] = this->shape.dimsSize[i];
				newShape.stride[i] = this->shape.stride[i];
			}
			newShape.dimsSize[operDim] = ( end - start ) / step + 1;
			newShape.size *= newShape.dimsSize[operDim];
			newShape.stride[operDim] *= step;
			return *new Tensor( std::move( newShape ), this->data, this->size );
		}
		this->shape.size /= this->shape.dimsSize[operDim];
		this->shape.dimsSize[operDim] = ( end - start ) / step + 1;
		this->shape.size *= this->shape.dimsSize[operDim];
		this->shape.offset += start * this->shape.stride[operDim];
		this->shape.stride[operDim] *= step;
		return *this;
	}
	// 如果已经是视图，直接修改原shape
	Tensor &operator[]( const uint i )
	{
		if ( i >= this->shape.dimsSize[0] )
			throw std::runtime_error( "Tensor: index out of range" );
		if ( this->shape.isnView )
		{
			TensorShape newShape(
			    false,
			    this->shape.dim - 1,
			    this->shape.size / this->shape.dimsSize[0],
			    this->shape.offset + i * this->shape.stride[0],
			    this->shape.dimsSize + 1,
			    this->shape.stride + 1 );
			return *new Tensor( std::move( newShape ), this->data, this->size );
		}
		this->shape.size /= this->shape.dimsSize[0];
		this->shape.offset += i * this->shape.stride[0];
		--this->shape.dim;
		++this->shape.dimsSize;
		++this->shape.stride;
		return *this;
	}
	Tensor &transpose( uint dim1 = 1, uint dim2 = 2 )
	{
		--dim1, --dim2;
		std::swap( this->shape.stride[dim1], this->shape.stride[dim2] );
		std::swap( this->shape.dimsSize[dim1], this->shape.dimsSize[dim2] );
		return *this;
	}
	// only copy the data in shape
	Tensor &copy()
	{
		TensorShape newShape( this->shape.dimsSize, this->shape.dim );
		Tensor *t = new Tensor( std::move( newShape ) );
		ull index, k;
		uint j;
		uint dim = this->shape.dim;
		ull *tstride = this->shape.stride;
		ull *stride = t->shape.stride;
		for ( ull i = 0; i < t->size; ++i )
		{
			k = i;
			index = this->shape.offset;
			for ( j = 0; j < dim; ++j )
			{
				index += k / stride[j] * tstride[j];
				k %= stride[j];
			}
			t->data[i] = this->data[index];
		}
		return *t;
	}
	template < uint N >
	T &dataOper( const uint ( &a )[N] )
	{
		if ( N != this->shape.dim )
			throw std::runtime_error( "Tensor: dim not match" );
		uint index = shape.offset;
		for ( uint i = 0; i < N; ++i )
			index += a[i] * shape.stride[i];
		return data[index];
	}
	T &oneValue()
	{
		// if ( this->shape.dim )
		// 	throw std::runtime_error( "Tensor: can't convert to scalar" );
		return this->data[this->shape.offset];
	}
	template < typename U, typename Caster >
	U &oneValue( Caster caster )
	{
		// if ( this->shape.dim )
		// throw std::runtime_error( "Tensor: can't convert to scalar" );
		return caster( this->data[this->shape.offset] );
	}
	~Tensor()
	{
		if ( this->shape.isnView && this->shape.isnSlice )
			delete[] this->data;
	}
};
#define data( a... ) dataOper( { a } )

int main( int argc, char const *argv[] )
{
	// TensorShape< 2 > shape( { 1, 2 } );
	Tensor< int > t( { 3, 4 } );
	int a = 0;
	for ( uint j = 0; j < 3; ++j )
		for ( uint k = 0; k < 4; ++k )
			t[j][k].oneValue() = ++a;
	// std::cout << t.shape << std::endl;

	for ( uint j = 0; j < 3; ++j )
	{
		for ( uint k = 0; k < 4; ++k )
			std::cout << t[j][k].oneValue() << " ";
		std::cout << std::endl;
	}
	auto t1 = t.transpose().copy();
	std::cout << std::endl
	          << t.shape
	          << std::endl
	          << t1.shape << std::endl;

	for ( uint j = 0; j < 4; ++j )
	{
		for ( uint k = 0; k < 3; ++k )
			std::cout << t1 /*( j, j, 1, 1 )( k, k, 1, 2 )*/.data( j, k ) << " ";
		std::cout << std::endl;
	}


	// for ( uint j = 0; j < 4; ++j )
	// {
	// 	for ( uint k = 0; k < 3; ++k )
	// 		std::cout << t[j][k].oneValue() << " ";
	// 	std::cout << std::endl;
	// }
	return 0;
}
