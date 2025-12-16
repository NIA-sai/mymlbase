#pragma once

#include "tensor.cc"


#define TENSOR_OPER_BROADCAST                                                                                              \
	const Tensor< T > *bigger_p, *smaller_p;                                                                               \
	uint result_dim;                                                                                                       \
	if ( this->shape.dim > other.shape.dim )                                                                               \
	{                                                                                                                      \
		bigger_p = this;                                                                                                   \
		smaller_p = &other;                                                                                                \
		result_dim = this->shape.dim;                                                                                      \
	}                                                                                                                      \
	else                                                                                                                   \
	{                                                                                                                      \
		bigger_p = &other;                                                                                                 \
		smaller_p = this;                                                                                                  \
		result_dim = other.shape.dim;                                                                                      \
	}                                                                                                                      \
	uint *result_dimsSize = new uint[result_dim], i = 0, dif = bigger_p->shape.dim - smaller_p->shape.dim, bigger_dimSize; \
	for ( ; i < dif; ++i )                                                                                                 \
		result_dimsSize[i] = bigger_p->shape.dimsSize[i];                                                                  \
	for ( ; i < result_dim; ++i )                                                                                          \
	{                                                                                                                      \
		bigger_dimSize = smaller_p->shape.dimsSize[i - dif];                                                               \
		if ( bigger_dimSize < bigger_p->shape.dimsSize[i] )                                                                \
		{                                                                                                                  \
			if ( bigger_dimSize != 1 )                                                                                     \
				throw std::runtime_error( "Dimensions do not match" );                                                     \
			bigger_dimSize = bigger_p->shape.dimsSize[i];                                                                  \
		}                                                                                                                  \
		else if ( bigger_dimSize > bigger_p->shape.dimsSize[i] && bigger_p->shape.dimsSize[i] != 1 )                       \
			throw std::runtime_error( "Dimensions do not match" );                                                         \
		result_dimsSize[i] = bigger_dimSize;                                                                               \
	}                                                                                                                      \
	Tensor< T > result = Tensor< T >( TensorShape( result_dimsSize, result_dim ) );                                        \
	delete[] result_dimsSize;

template < typename T >
void dfs_add( Tensor< T > *result, const Tensor< T > *bigger_p, const Tensor< T > *smaller_p, const uint dim, const uint dim_index, const ull r_r_data_index, const ull b_r_data_index, const ull s_r_data_index, const uint dif )
{
	if ( dim == dim_index )
		result->r_data[r_r_data_index] = bigger_p->r_data[b_r_data_index] + smaller_p->r_data[s_r_data_index];
	else if ( dim_index < dif )
		for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
			dfs_add( result, bigger_p, smaller_p, dim, dim_index + 1,
			         r_r_data_index + i * ( result->shape.stride[dim_index] ),
			         b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
			         s_r_data_index,
			         dif );
	else
	{
		if ( result->shape.dimsSize[dim_index] == 1 )
			dfs_add( result, bigger_p, smaller_p, dim, dim_index + 1, r_r_data_index, b_r_data_index, s_r_data_index, dif );
		else if ( bigger_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_add( result, bigger_p, smaller_p, dim, dim_index + 1,
				         r_r_data_index + i * ( result->shape.stride[dim_index] ),
				         b_r_data_index,
				         s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				         dif );
		else if ( smaller_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_add( result, bigger_p, smaller_p, dim, dim_index + 1,
				         r_r_data_index + i * ( result->shape.stride[dim_index] ),
				         b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				         s_r_data_index,
				         dif );
		else
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_add( result, bigger_p, smaller_p, dim, dim_index + 1,
				         r_r_data_index + i * ( result->shape.stride[dim_index] ),
				         b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				         s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				         dif );
	}
}
template < typename T >
void dfs_sub1( Tensor< T > *result, const Tensor< T > *bigger_p, const Tensor< T > *smaller_p, const uint dim, const uint dim_index, const ull r_r_data_index, const ull b_r_data_index, const ull s_r_data_index, const uint dif )
{
	if ( dim == dim_index )
		result->r_data[r_r_data_index] = bigger_p->r_data[b_r_data_index] - smaller_p->r_data[s_r_data_index];
	else if ( dim_index < dif )
		for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
			dfs_sub1( result, bigger_p, smaller_p, dim, dim_index + 1,
			          r_r_data_index + i * ( result->shape.stride[dim_index] ),
			          b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
			          s_r_data_index,
			          dif );
	else
	{
		if ( result->shape.dimsSize[dim_index] == 1 )
			dfs_sub1( result, bigger_p, smaller_p, dim, dim_index + 1, r_r_data_index, b_r_data_index, s_r_data_index, dif );
		else if ( bigger_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_sub1( result, bigger_p, smaller_p, dim, dim_index + 1,
				          r_r_data_index + i * ( result->shape.stride[dim_index] ),
				          b_r_data_index,
				          s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				          dif );
		else if ( smaller_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_sub1( result, bigger_p, smaller_p, dim, dim_index + 1,
				          r_r_data_index + i * ( result->shape.stride[dim_index] ),
				          b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				          s_r_data_index,
				          dif );
		else
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_sub1( result, bigger_p, smaller_p, dim, dim_index + 1,
				          r_r_data_index + i * ( result->shape.stride[dim_index] ),
				          b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				          s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				          dif );
	}
}

template < typename T >
void dfs_sub2( Tensor< T > *result, const Tensor< T > *bigger_p, const Tensor< T > *smaller_p, const uint dim, const uint dim_index, const ull r_r_data_index, const ull b_r_data_index, const ull s_r_data_index, const uint dif )
{
	if ( dim == dim_index )
		result->r_data[r_r_data_index] = smaller_p->r_data[b_r_data_index] - bigger_p->r_data[s_r_data_index];
	else if ( dim_index < dif )
		for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
			dfs_sub2( result, bigger_p, smaller_p, dim, dim_index + 1,
			          r_r_data_index + i * ( result->shape.stride[dim_index] ),
			          b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
			          s_r_data_index,
			          dif );
	else
	{
		if ( result->shape.dimsSize[dim_index] == 1 )
			dfs_sub2( result, bigger_p, smaller_p, dim, dim_index + 1, r_r_data_index, b_r_data_index, s_r_data_index, dif );
		else if ( bigger_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_sub2( result, bigger_p, smaller_p, dim, dim_index + 1,
				          r_r_data_index + i * ( result->shape.stride[dim_index] ),
				          b_r_data_index,
				          s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				          dif );
		else if ( smaller_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_sub2( result, bigger_p, smaller_p, dim, dim_index + 1,
				          r_r_data_index + i * ( result->shape.stride[dim_index] ),
				          b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				          s_r_data_index,
				          dif );
		else
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_sub2( result, bigger_p, smaller_p, dim, dim_index + 1,
				          r_r_data_index + i * ( result->shape.stride[dim_index] ),
				          b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				          s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				          dif );
	}
}

template < typename T >
Tensor< T > Tensor< T >::operator+( const Tensor< T > &other ) const
{
	TENSOR_OPER_BROADCAST
	dfs_add( &result, bigger_p, smaller_p, result_dim, 0, 0, bigger_p->shape.offset, smaller_p->shape.offset, dif );
	return result;
}
template < typename T >
Tensor< T > Tensor< T >::operator-() const
{
	TensorShape newShape( this->shape.dimsSize, this->shape.dim );
	Tensor< T > t( std::move( newShape ) );
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
		t.r_data[i] = ( -this->r_data[index] );
	}
	return t;
}
template < typename T >
Tensor< T > Tensor< T >::operator-( const Tensor< T > &other ) const
{
	TENSOR_OPER_BROADCAST
	bigger_p == this ? dfs_sub1( &result, bigger_p, smaller_p, result_dim, 0, 0, bigger_p->shape.offset, smaller_p->shape.offset, dif )
	                 : dfs_sub2( &result, bigger_p, smaller_p, result_dim, 0, 0, bigger_p->shape.offset, smaller_p->shape.offset, dif );
	return result;
}
template < typename T >
Tensor< T > Tensor< T >::operator*( const Tensor< T > &other ) const
{
	if ( this->shape.dim == 0 )
		return other * ( this->oneValue() );
	if ( other.shape.dim == 0 )
		return this->operator*( other.oneValue() );
	if ( this->shape.dim > 2 || other.shape.dim > 2 )
		throw std::runtime_error( "Tensor Operator * : haven't implemented for tensors with dim > 2 yet." );
	if ( this->shape.dim == 2 && other.shape.dim == 2 )
	{
		if ( this->shape.dimsSize[1] != other.shape.dimsSize[0] )
			throw std::runtime_error( "Tensor Operator * : matrixs dim not match." );
		Tensor result( { this->shape.dimsSize[0], other.shape.dimsSize[1] } );
		ull index;
		for ( uint i = 0; i < this->shape.dimsSize[0]; ++i )
			for ( uint j = 0; j < other.shape.dimsSize[1]; ++j )
			{
				index = i * other.shape.dimsSize[1] + j;
				result.r_data[index] = 0;
				for ( uint k = 0; k < this->shape.dimsSize[1]; ++k )
					result.r_data[index] += this->r_data[this->shape.offset + i * this->shape.stride[0] + k * this->shape.stride[1]] * other.r_data[other.shape.offset + k * other.shape.stride[0] + j * other.shape.stride[1]];
			}
		return result;
	}
	else if ( this->shape.dim == 1 && other.shape.dim == 1 )
	{
		if ( this->shape.dimsSize[0] != other.shape.dimsSize[0] )
			throw std::runtime_error( "Tensor Operator * : vectors must have the same size." );
		Tensor result( { this->shape.dimsSize[0], this->shape.dimsSize[0] } );
		for ( uint i = 0; i < this->shape.dimsSize[0]; ++i )
			for ( uint j = 0; j < this->shape.dimsSize[0]; ++j )
				result.r_data[i * this->shape.dimsSize[0] + j] = this->r_data[this->shape.offset + i * this->shape.stride[0]] * other.r_data[other.shape.offset + j * other.shape.stride[0]];
		return result;
	}
	else if ( this->shape.dim == 1 )
	{
		if ( this->shape.dimsSize[0] != other.shape.dimsSize[0] )
			throw std::runtime_error( "Tensor Operator * (v*m): vector must have the same size of matrix's first dim." );
		Tensor result( { other.shape.dimsSize[1] } );
		for ( uint i = 0; i < other.shape.dimsSize[1]; ++i )
		{
			result.r_data[i] = 0;
			for ( uint j = 0; j < this->shape.dimsSize[0]; ++j )
				result.r_data[i] += this->r_data[this->shape.offset + j * this->shape.stride[0]] * other.r_data[other.shape.offset + j * other.shape.stride[0] + i * other.shape.stride[1]];
		}
		return result;
	}
	else
	{
		if ( this->shape.dimsSize[1] != other.shape.dimsSize[0] )
			throw std::runtime_error( "Tensor Operator * (m*v): vector must have the same size of matrix's second dim." );
		Tensor result( { this->shape.dimsSize[0] } );
		for ( uint i = 0; i < this->shape.dimsSize[0]; ++i )
		{
			result.r_data[i] = 0;
			for ( uint j = 0; j < this->shape.dimsSize[0]; ++j )
				result.r_data[i] += this->r_data[this->shape.offset + i * this->shape.stride[0] + j * this->shape.stride[1]] * other.r_data[other.shape.offset + j * other.shape.stride[0]];
		}
		return result;
	}
}
template < typename T >
T Tensor< T >::operator^( const Tensor< T > &other ) const
{
	if ( this->shape.dim > 1 || other.shape.dim > 1 )
		throw std::runtime_error( "Tensor Operator ^ : is only use for vectors." );
	if ( this->shape.dimsSize[0] != other.shape.dimsSize[0] )
		throw std::runtime_error( "Tensor Operator ^ : vectors must have the same size." );
	T result = 0;
	for ( ull i = 0; i < this->shape.dimsSize[0]; ++i )
		result += this->r_data[this->shape.offset + i * this->shape.stride[0]] * other.r_data[other.shape.offset + i * other.shape.stride[0]];
	return result;
}
template < typename T >
Tensor< T > Tensor< T >::operator/( const Tensor< T > &other ) const
{
}
template < typename T >
Tensor< T > Tensor< T >::operator+( const T &scalar ) const
{
}
template < typename T >
Tensor< T > Tensor< T >::operator-( const T &scalar ) const
{
}
template < typename T >
Tensor< T > Tensor< T >::operator*( const T &scalar ) const
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
		t.r_data[i] = ( this->r_data[index] * scalar );
	}
	return t;
}
template < typename T >
Tensor< T > Tensor< T >::operator/( const T &scalar ) const
{
	if ( abs( scalar ) <= TENSOR_SCALAR_ZERO )
		throw std::runtime_error( "Tensor Operator / : scalar is zero." );
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
		t.r_data[i] = ( this->r_data[index] / scalar );
	}
	return t;
}
template < typename T >
Tensor< T > &Tensor< T >::operator+=( const Tensor< T > &other )
{
	this->operator=( this->operator+( other ) );
	return *this;
}
template < typename T >
Tensor< T > &Tensor< T >::operator-=( const Tensor< T > &other )
{
	this->operator=( this->operator-( other ) );
	return *this;
}
template < typename T >
Tensor< T > &Tensor< T >::operator*=( const Tensor< T > &other )
{
	this->operator=( this->operator*( other ) );
	return *this;
}
template < typename T >
Tensor< T > &Tensor< T >::operator/=( const Tensor< T > &other )
{
	this->operator=( this->operator/( other ) );
	return *this;
}
template < typename T >
Tensor< T > &Tensor< T >::operator+=( const T &scalar )
{
}
template < typename T >
Tensor< T > &Tensor< T >::operator-=( const T &scalar )
{
}
template < typename T >
Tensor< T > &Tensor< T >::operator*=( const T &scalar )
{
	this->operator=( this->operator*( scalar ) );
	return *this;
}
template < typename T >
Tensor< T > &Tensor< T >::operator/=( const T &scalar )
{
	this->operator=( this->operator/( scalar ) );
	return *this;
}