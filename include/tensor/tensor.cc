#pragma once
#include "../base.hpp"
#include "../oper/oper.hpp"
#include "../utils/not.hpp"
#include "../utils/sto_func_map.cpp"
#include <iostream>
#define 𤳳 Tensor
#ifndef TENSOR_GRAD_TYPE
	#define TENSOR_GRAD_TYPE double
#endif

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
	bool operator==( const TensorShape &t ) const
	{
		if ( dim != t.dim )
			return false;
		for ( uint i = 0; i < dim; ++i )
			if ( dimsSize[i] != t.dimsSize[i] )
				return false;
		return true;
	}
	bool operator!=( const TensorShape &t ) const { return !( *this == t ); }
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
	T *r_data = nullptr;
	// really size
	ull size = 0;
	TensorShape shape;

	Tensor< TENSOR_GRAD_TYPE > *grad = nullptr;
	bool needGrad = false;
	Oper *creator = nullptr;
	Tensor() = default;
	Tensor( const TensorShape &shape, const T *r_data = nullptr, bool needGrad = false, Oper *creator = nullptr ) : shape( shape ), needGrad( needGrad ), creator( creator )
	{
		this->r_data = new T[shape.size];
		this->size = shape.size;
		if ( r_data )
		{
			for ( ull i = 0; i < shape.size; ++i )
				this->r_data[i] = r_data[i];
		}
	}
	template < uint N = 2 >
	Tensor( const uint ( &dimsSize )[N] = { 3, 3 }, const T *r_data = nullptr, bool needGrad = false, Oper *creator = nullptr )
	    : Tensor( TensorShape( dimsSize, N ), r_data, needGrad, creator ) {}
	// to get view
	Tensor( TensorShape &&shape, T *r_data, ull size, bool needGrad = false, Tensor< TENSOR_GRAD_TYPE > *grad = nullptr, Oper *creator = nullptr ) : shape( std::move( shape ) ), r_data( r_data ), size( size ), needGrad( needGrad ), grad( grad ), creator( creator )
	{
		// this->shape.isnView = false;
	}
	// caution!! 暂时不要在view上使用，最好先copy
	void reshape( const TensorShape &shape )
	{
		this->shape = shape;
	}
	// caution!! 暂时不要在view上使用，最好先copy
	template < uint N = 2 >
	void reshape( const uint ( &dimsSize )[N] )
	{
		this->reshape( TensorShape( dimsSize, N ) );
	}
	// copy() not copy creater and grad
	Tensor( TensorShape &&shape ) : shape( std::move( shape ) ), size( shape.size ), r_data( new T[shape.size] ) {}
	Tensor( const Tensor &t ) : shape( t.shape ), size( t.size ), needGrad( t.needGrad ), grad( t.grad ), creator( t.creator )  // creater->transer?
	{
		if ( t.shape.isnView )
		{
			this->r_data = new T[t.size];
			for ( ull i = 0; i < t.size; ++i )
				this->r_data[i] = T( t.r_data[i] );
		}
		else
			this->r_data = t.r_data;
	}
	Tensor( Tensor &&t ) : shape( std::move( t.shape ) ), r_data( t.r_data ), size( t.size ), needGrad( t.needGrad ), creator( t.creator )
	{
		t.r_data = nullptr;
	}

	// template < typename T2 >
	// operator Tensor< T2 >()
	// {  // todo
	// 	return Tensor< T2 >( this->shape, this->r_data, this->needGrad, this->creator );
	// }

	Tensor &operator=( const Tensor &t )
	{
		if ( this != &t )
		{
			if ( this->shape.isnView && this->shape.isnSlice )
				delete[] this->r_data;
			this->shape = t.shape;
			this->size = t.size;
			this->needGrad = t.needGrad;
			this->creator = t.creator;
			if ( t.shape.isnView )
			{
				this->r_data = new T[t.shape.size];
				for ( ull i = 0; i < t.shape.size; ++i )
					this->r_data[i] = t.r_data[i];
			}
			else
				this->r_data = t.r_data;
		}
		return *this;
	}
	Tensor &operator=( Tensor &&t )
	{
		if ( this != &t )
		{
			if ( this->shape.isnView && this->shape.isnSlice )
				delete[] this->r_data;
			this->shape = std::move( t.shape );
			this->size = t.size;
			this->r_data = t.r_data;
			this->needGrad = t.needGrad;
			this->creator = t.creator;
			t.r_data = nullptr;
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
			return *new Tensor( std::move( newShape ), this->r_data, this->size );
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
		if ( this->shape.dim == 0 || i >= this->shape.dimsSize[0] )
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
			return *new Tensor( std::move( newShape ), this->r_data, this->size );
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
	// only copy the r_data in shape

	Tensor copy()
	{
		TensorShape newShape( this->shape.dimsSize, this->shape.dim );
		Tensor t( std::move( newShape ) );
		ull index, k;
		uint j;
		uint dim = this->shape.dim;
		ull *tstride = this->shape.stride;
		ull *stride = t.shape.stride;
		for ( ull i = 0; i < t.size; ++i )
		{
			k = i;
			index = this->shape.offset;
			for ( j = 0; j < dim; ++j )
			{
				index += k / stride[j] * tstride[j];
				k %= stride[j];
			}
			t.r_data[i] = this->r_data[index];
		}
		return t;
	}
	template < typename U >
	Tensor< U > to()
	{
		TensorShape newShape( this->shape.dimsSize, this->shape.dim );
		Tensor t( std::move( newShape ) );
		ull index, k;
		uint j;
		uint dim = this->shape.dim;
		ull *tstride = this->shape.stride;
		ull *stride = t.shape.stride;
		for ( ull i = 0; i < t.size; ++i )
		{
			k = i;
			index = this->shape.offset;
			for ( j = 0; j < dim; ++j )
			{
				index += k / stride[j] * tstride[j];
				k %= stride[j];
			}
			t.r_data[i] = U( this->r_data[index] );
		}
		return t;
	}
	template < uint N >
	T &dataOper( const uint ( &a )[N] ) const
	{
		if ( ( this->shape.dim > 0 && N != this->shape.dim ) )
			throw std::runtime_error( "Tensor: dim not match" );
		if ( this->shape.dim == 0 )
			return this->r_data[this->shape.offset];
		uint index = shape.offset;
		for ( uint i = 0; i < N; ++i )
			index += a[i] * shape.stride[i];
		return r_data[index];
	}
	template < typename... Args >
	T &data( Args... args ) const
	{
		return dataOper( { static_cast< uint >( args )... } );
	}
	T &oneValue()
	{
		// if ( this->shape.dim )
		// 	throw std::runtime_error( "Tensor: can't convert to scalar" );
		return this->r_data[this->shape.offset];
	}
	template < typename U, typename Caster >
	U &oneValue( Caster caster )
	{
		// if ( this->shape.dim )
		// throw std::runtime_error( "Tensor: can't convert to scalar" );
		return caster( this->r_data[this->shape.offset] );
	}

	void clearGrad()
	{
		if ( !this->needGrad )
			return;
		this->creator->clearGrad();
		delete this->grad;
		this->grad = new Tensor( this->shape );
		for ( ull i = 0; i < this->size; ++i )
			this->grad->r_data[i] = 0;
	}
	void backward()
	{
		if ( this->shape.size != 1 )
			throw std::runtime_error( "Tensor: can't backward/has not implemented yet" );
		if ( !this->needGrad )
			return;
		this->creator->calGrad();
		this->grad = Tensor< TENSOR_GRAD_TYPE >::InitWithOneValue( this->shape, 1 );
		this->creator->backward();
	}
	static Tensor *InitWithOneValue( const TensorShape &shape, T value, bool needGrad = false, Oper *creator = nullptr )
	{
		Tensor *t = new Tensor( shape, nullptr, needGrad, creator );
		for ( ull i = 0; i < shape.size; ++i )
			t->r_data[i] = value;
	}

	~Tensor()
	{
		if ( this->shape.isnView && this->shape.isnSlice )
			delete[] this->r_data;
	}
	friend std::ostream &operator<<( std::ostream &os, Tensor &t )
	{
		if ( t.shape.dim == 0 )
			return os << t.r_data[t.shape.offset] << ",";
		if ( t.shape.dim == 1 )
		{
			os << "[";
			for ( uint i = 0; i < t.shape.dimsSize[0]; ++i )
				os << t.r_data[t.shape.offset + i * t.shape.stride[0]] << ",";
			os << "]\n";
			return os;
		}
		os << "[";
		for ( uint i = 0; i < t.shape.dimsSize[0]; ++i )
		{
			os << t[i];
		}
		os << "]\n";
		return os;
	}
};
#define t_data( a... ) dataOper( { a } )

template <>
template < typename U >
Tensor< U > Tensor< std::string >::to()
{
	TensorShape newShape( this->shape.dimsSize, this->shape.dim );
	Tensor< U > t( std::move( newShape ) );
	ull index, k;
	uint j;
	uint dim = this->shape.dim;
	ull *tstride = this->shape.stride;
	ull *stride = t.shape.stride;
	for ( ull i = 0; i < t.size; ++i )
	{
		k = i;
		index = this->shape.offset;
		for ( j = 0; j < dim; ++j )
		{
			index += k / stride[j] * tstride[j];
			k %= stride[j];
		}
		t.r_data[i] = sto< U >( this->r_data[index] );
	}
	return t;
}
// int test( int argc, char const *argv[] )
// {
// 	// TensorShape< 2 > shape( { 1, 2 } );
// 	Tensor< int > t( { 3, 4 } );
// 	int a = 0;
// 	for ( uint j = 0; j < 3; ++j )
// 		for ( uint k = 0; k < 4; ++k )
// 			t[j][k].oneValue() = ++a;
// 	// std::cout << t.shape << std::endl;

// 	for ( uint j = 0; j < 3; ++j )
// 	{
// 		for ( uint k = 0; k < 4; ++k )
// 			std::cout << t[j][k].oneValue() << " ";
// 		std::cout << std::endl;
// 	}
// 	auto tp = t.transpose().copy();
// 	auto &t1 = *tp;
// 	std::cout << std::endl
// 	          << t.shape
// 	          << std::endl
// 	          << t1.shape << std::endl;

// 	for ( uint j = 0; j < 4; ++j )
// 	{
// 		for ( uint k = 0; k < 3; ++k )
// 			std::cout << t1 /*( j, j, 1, 1 )( k, k, 1, 2 )*/.data( j, k ) << " ";
// 		std::cout << std::endl;
// 	}


// for ( uint j = 0; j < 4; ++j )
// {
// 	for ( uint k = 0; k < 3; ++k )
// 		std::cout << t[j][k].oneValue() << " ";
// 	std::cout << std::endl;
// }
// return 0;
// }
