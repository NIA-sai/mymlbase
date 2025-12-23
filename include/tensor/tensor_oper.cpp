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
			{                                                                                                              \
				throw std::runtime_error( "tensor oper broadcast: dim do not match" );                                     \
			}                                                                                                              \
			bigger_dimSize = bigger_p->shape.dimsSize[i];                                                                  \
		}                                                                                                                  \
		else if ( bigger_dimSize > bigger_p->shape.dimsSize[i] && bigger_p->shape.dimsSize[i] != 1 )                       \
		{                                                                                                                  \
			cout << i << ":" << bigger_p->shape;                                                                           \
			cout << i << ":" << smaller_p->shape;                                                                          \
			throw std::runtime_error( "tensor oper broadcast: dim do not match1" );                                        \
		}                                                                                                                  \
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
		result->r_data[r_r_data_index] = smaller_p->r_data[s_r_data_index] - bigger_p->r_data[b_r_data_index];
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
void dfs_e_mul( Tensor< T > *result, const Tensor< T > *bigger_p, const Tensor< T > *smaller_p, const uint dim, const uint dim_index, const ull r_r_data_index, const ull b_r_data_index, const ull s_r_data_index, const uint dif )
{
	if ( dim == dim_index )
		result->r_data[r_r_data_index] = bigger_p->r_data[b_r_data_index] * smaller_p->r_data[s_r_data_index];
	else if ( dim_index < dif )
		for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
			dfs_e_mul( result, bigger_p, smaller_p, dim, dim_index + 1,
			           r_r_data_index + i * ( result->shape.stride[dim_index] ),
			           b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
			           s_r_data_index,
			           dif );
	else
	{
		if ( result->shape.dimsSize[dim_index] == 1 )
			dfs_e_mul( result, bigger_p, smaller_p, dim, dim_index + 1, r_r_data_index, b_r_data_index, s_r_data_index, dif );
		else if ( bigger_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_e_mul( result, bigger_p, smaller_p, dim, dim_index + 1,
				           r_r_data_index + i * ( result->shape.stride[dim_index] ),
				           b_r_data_index,
				           s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				           dif );
		else if ( smaller_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_e_mul( result, bigger_p, smaller_p, dim, dim_index + 1,
				           r_r_data_index + i * ( result->shape.stride[dim_index] ),
				           b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				           s_r_data_index,
				           dif );
		else
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_e_mul( result, bigger_p, smaller_p, dim, dim_index + 1,
				           r_r_data_index + i * ( result->shape.stride[dim_index] ),
				           b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				           s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				           dif );
	}
}

// todo :tmp
#include <cmath>

template < typename T >
void dfs_e_div1( Tensor< T > *result, const Tensor< T > *bigger_p, const Tensor< T > *smaller_p, const uint dim, const uint dim_index, const ull r_r_data_index, const ull b_r_data_index, const ull s_r_data_index, const uint dif )
{
	if ( dim == dim_index )
	{
		if ( smaller_p->r_data[s_r_data_index] == T( 0 ) )
		{
			result->r_data[r_r_data_index] = bigger_p->r_data[b_r_data_index] >= T( 0 ) ? MAX< T >() : -MAX< T >();
			return;
		}
		result->r_data[r_r_data_index] = bigger_p->r_data[b_r_data_index] / smaller_p->r_data[s_r_data_index];
		if ( std::isinf( result->r_data[r_r_data_index] ) )
		{
			if ( result->r_data[r_r_data_index] > 0 )
				result->r_data[r_r_data_index] = MAX< T >();
			else
				result->r_data[r_r_data_index] = -MAX< T >();
		}
		//( smaller_p->r_data[s_r_data_index] <= TENSOR_SCALAR_ZERO ) ? ( bigger_p->r_data[b_r_data_index] > 0 ? MAX< T >() : -MAX< T >() ) : bigger_p->r_data[b_r_data_index] / smaller_p->r_data[s_r_data_index];
	}
	else if ( dim_index < dif )
		for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
			dfs_e_div1( result, bigger_p, smaller_p, dim, dim_index + 1,
			            r_r_data_index + i * ( result->shape.stride[dim_index] ),
			            b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
			            s_r_data_index,
			            dif );
	else
	{
		if ( result->shape.dimsSize[dim_index] == 1 )
			dfs_e_div1( result, bigger_p, smaller_p, dim, dim_index + 1, r_r_data_index, b_r_data_index, s_r_data_index, dif );
		else if ( bigger_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_e_div1( result, bigger_p, smaller_p, dim, dim_index + 1,
				            r_r_data_index + i * ( result->shape.stride[dim_index] ),
				            b_r_data_index,
				            s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				            dif );
		else if ( smaller_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_e_div1( result, bigger_p, smaller_p, dim, dim_index + 1,
				            r_r_data_index + i * ( result->shape.stride[dim_index] ),
				            b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				            s_r_data_index,
				            dif );
		else
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_e_div1( result, bigger_p, smaller_p, dim, dim_index + 1,
				            r_r_data_index + i * ( result->shape.stride[dim_index] ),
				            b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				            s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				            dif );
	}
}

template < typename T >
void dfs_e_div2( Tensor< T > *result, const Tensor< T > *bigger_p, const Tensor< T > *smaller_p, const uint dim, const uint dim_index, const ull r_r_data_index, const ull b_r_data_index, const ull s_r_data_index, const uint dif )
{
	if ( dim == dim_index )
	{
		if ( bigger_p->r_data[b_r_data_index] == T( 0 ) )
		{
			result->r_data[r_r_data_index] = smaller_p->r_data[s_r_data_index] >= T( 0 ) ? MAX< T >() : -MAX< T >();
			return;
		}
		result->r_data[r_r_data_index] = smaller_p->r_data[s_r_data_index] / bigger_p->r_data[b_r_data_index];
		if ( std::isinf( result->r_data[r_r_data_index] ) )
		{
			if ( result->r_data[r_r_data_index] > 0 )
				result->r_data[r_r_data_index] = MAX< T >();
			else
				result->r_data[r_r_data_index] = -MAX< T >();
		}
	}

	else if ( dim_index < dif )
		for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
			dfs_e_div2( result, bigger_p, smaller_p, dim, dim_index + 1,
			            r_r_data_index + i * ( result->shape.stride[dim_index] ),
			            b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
			            s_r_data_index,
			            dif );
	else
	{
		if ( result->shape.dimsSize[dim_index] == 1 )
			dfs_e_div2( result, bigger_p, smaller_p, dim, dim_index + 1, r_r_data_index, b_r_data_index, s_r_data_index, dif );
		else if ( bigger_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_e_div2( result, bigger_p, smaller_p, dim, dim_index + 1,
				            r_r_data_index + i * ( result->shape.stride[dim_index] ),
				            b_r_data_index,
				            s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				            dif );
		else if ( smaller_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_e_div2( result, bigger_p, smaller_p, dim, dim_index + 1,
				            r_r_data_index + i * ( result->shape.stride[dim_index] ),
				            b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				            s_r_data_index,
				            dif );
		else
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_e_div2( result, bigger_p, smaller_p, dim, dim_index + 1,
				            r_r_data_index + i * ( result->shape.stride[dim_index] ),
				            b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				            s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				            dif );
	}
}
// todo
// template < typename T >
// Tensor< T > Tensor< T >::Mul( const Tensor< T > &a, const Tensor< T > &b, const uint (&a_dim)[], const uint (&b_dim)[] )
// {
// 	if ( a.shape.dimsSize[a_col_dim] != b.shape.dimsSize[b_row_dim] )
// 		throw std::runtime_error( "Tensor Mul2D : matrixs dim not match." );
// 	Tensor result( { this->shape.dimsSize[0], other.shape.dimsSize[1] } );
// 	ull index;
// 	for ( uint i = 0; i < this->shape.dimsSize[0]; ++i )
// 		for ( uint j = 0; j < other.shape.dimsSize[1]; ++j )
// 		{
// 			index = i * other.shape.dimsSize[1] + j;
// 			result.r_data[index] = 0;
// 			for ( uint k = 0; k < this->shape.dimsSize[1]; ++k )
// 				result.r_data[index] += this->r_data[this->shape.offset + i * this->shape.stride[0] + k * this->shape.stride[1]] * other.r_data[other.shape.offset + k * other.shape.stride[0] + j * other.shape.stride[1]];
// 		}
// 	return result;
// }

template < typename T >
Tensor< T > Tensor< T >::operator+( const Tensor< T > &other ) const
{
	if ( this->shape.dim == 0 )
		return other + ( this->oneValue() );
	if ( other.shape.dim == 0 )
		return this->operator+( other.oneValue() );
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
	if ( other.shape.dim == 0 )
		return this->operator-( other.oneValue() );
	TENSOR_OPER_BROADCAST
	bigger_p == this ? dfs_sub1( &result, bigger_p, smaller_p, result_dim, 0, 0, bigger_p->shape.offset, smaller_p->shape.offset, dif )
	                 : dfs_sub2( &result, bigger_p, smaller_p, result_dim, 0, 0, bigger_p->shape.offset, smaller_p->shape.offset, dif );
	return result;
}

template < typename T >
Tensor< T > Tensor< T >::eMul( const Tensor< T > &other ) const
{
	TENSOR_OPER_BROADCAST
	dfs_e_mul( &result, bigger_p, smaller_p, result_dim, 0, 0, bigger_p->shape.offset, smaller_p->shape.offset, dif );
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
		{
			cout << this->shape << other.shape << endl;
			throw std::runtime_error( "Tensor Operator * : matrixs dim not match." );
		}
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
	// if ( this->shape.dim > 1 || other.shape.dim > 1 )
	// 	throw std::runtime_error( "Tensor Operator ^ : is only use for vectors." );
	// if ( this->shape.dimsSize[0] != other.shape.dimsSize[0] )
	// 	throw std::runtime_error( "Tensor Operator ^ : vectors must have the same size." );
	if ( this->shape != other.shape )
		throw std::runtime_error( "Tensor Operator ^ : tensor must have the same size." );
	T result = 0;
	if ( this->shape.dim == 1 )
	{
		for ( uint i = 0; i < this->shape.dimsSize[0]; ++i )
			result += this->r_data[this->shape.offset + i * this->shape.stride[0]] * other.r_data[other.shape.offset + i * other.shape.stride[0]];
		return result;
	}
	for ( ull i = 0; i < this->shape.dimsSize[0]; ++i )
		result += this->operator[]( i ) ^ other.operator[]( i );
	return result;
}
template < typename T >
Tensor< T > Tensor< T >::operator/( const Tensor< T > &other ) const
{
	if ( other.shape.dim == 0 )
		return this->operator/( other.oneValue() );
	TENSOR_OPER_BROADCAST
	bigger_p == this ? dfs_e_div1( &result, bigger_p, smaller_p, result_dim, 0, 0, bigger_p->shape.offset, smaller_p->shape.offset, dif )
	                 : dfs_e_div2( &result, bigger_p, smaller_p, result_dim, 0, 0, bigger_p->shape.offset, smaller_p->shape.offset, dif );
	return result;
}

#define TENSOR_OPER_SCALER_AMMD_LOGIC( op )                        \
	TensorShape newShape( this->shape.dimsSize, this->shape.dim ); \
	Tensor t( std::move( newShape ) );                             \
	ull index, k;                                                  \
	uint j;                                                        \
	uint dim = this->shape.dim;                                    \
	ull *tstride = this->shape.stride;                             \
	ull *stride = t.shape.stride;                                  \
	for ( ull i = 0; i < t.size; ++i )                             \
	{                                                              \
		k = i;                                                     \
		index = this->shape.offset;                                \
		for ( j = 0; j < dim; ++j )                                \
		{                                                          \
			index += k / stride[j] * tstride[j];                   \
			k %= stride[j];                                        \
		}                                                          \
		t.r_data[i] = ( this->r_data[index] op scalar );           \
	}                                                              \
	return t;
#define TENSOR_OPER_SCALER_LBE_LOGIC( op )                                 \
	TensorShape newShape( this->shape.dimsSize, this->shape.dim );         \
	Tensor t( std::move( newShape ) );                                     \
	ull index, k;                                                          \
	uint j;                                                                \
	uint dim = this->shape.dim;                                            \
	ull *tstride = this->shape.stride;                                     \
	ull *stride = t.shape.stride;                                          \
	for ( ull i = 0; i < t.size; ++i )                                     \
	{                                                                      \
		k = i;                                                             \
		index = this->shape.offset;                                        \
		for ( j = 0; j < dim; ++j )                                        \
		{                                                                  \
			index += k / stride[j] * tstride[j];                           \
			k %= stride[j];                                                \
		}                                                                  \
		t.r_data[i] = ( this->r_data[index] op scalar ) ? T( 1 ) : T( 0 ); \
	}                                                                      \
	return t;
#define TENSOR_OPER_SCALER_AMM( op )                               \
	template < typename T >                                        \
	Tensor< T > Tensor< T >::operator op( const T & scalar ) const \
	{                                                              \
		TENSOR_OPER_SCALER_AMMD_LOGIC( op )                        \
	}
TENSOR_OPER_SCALER_AMM( +)
TENSOR_OPER_SCALER_AMM( -)
template < typename T >
Tensor< T > operator-( const T &scalar, const Tensor< T > &tensor )
{
	TensorShape newShape( tensor.shape.dimsSize, tensor.shape.dim );
	Tensor t( std::move( newShape ) );
	ull index, k;
	uint j;
	uint dim = tensor.shape.dim;
	ull *tstride = tensor.shape.stride;
	ull *stride = t.shape.stride;
	for ( ull i = 0; i < t.size; ++i )
	{
		k = i;
		index = tensor.shape.offset;
		for ( j = 0; j < dim; ++j )
		{
			index += k / stride[j] * tstride[j];
			k %= stride[j];
		}
		t.r_data[i] = scalar - tensor.r_data[index];
	}
	return t;
}
template < typename T >
Tensor< T > operator/( const T &scalar, const Tensor< T > &tensor )
{
	TensorShape newShape( tensor.shape.dimsSize, tensor.shape.dim );
	Tensor t( std::move( newShape ) );
	ull index, k;
	uint j;
	uint dim = tensor.shape.dim;
	ull *tstride = tensor.shape.stride;
	ull *stride = t.shape.stride;
	for ( ull i = 0; i < t.size; ++i )
	{
		k = i;
		index = tensor.shape.offset;
		for ( j = 0; j < dim; ++j )
		{
			index += k / stride[j] * tstride[j];
			k %= stride[j];
		}
		t.r_data[i] = scalar / tensor.r_data[index];
	}
	return t;
}
TENSOR_OPER_SCALER_AMM( * )

// template < typename T >
// Tensor< T > Tensor< T >::operator+( const T &scalar ) const
// {
// }
// template < typename T >
// Tensor< T > Tensor< T >::operator-( const T &scalar ) const
// {
// 	TENSOR_OPER_SCALER_AMMD( -)
// }
// template < typename T >
// Tensor< T > Tensor< T >::operator*( const T &scalar ) const
// {
// 	TENSOR_OPER_SCALER_AMMD( * )
// }
template < typename T >
Tensor< T > Tensor< T >::operator/( const T &scalar ) const
{
	if ( std::abs( scalar ) <= TENSOR_SCALAR_ZERO )
		throw std::runtime_error( "Tensor Operator / : scalar is zero." );
	TENSOR_OPER_SCALER_AMMD_LOGIC( / )
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
	if ( scalar == 0 )
		return *this;
	this->operator=( this->operator-( scalar ) );
	return *this;
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
template < typename T >
Tensor< T > Tensor< T >::nPow( const uint &pow ) const
{
	TensorShape newShape( this->shape.dimsSize, this->shape.dim );
	Tensor t( std::move( newShape ) );
	ull index, k;
	uint j, p;
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
		t.r_data[i] = 1;
		for ( p = 0; p < pow; ++p )
			t.r_data[i] *= this->r_data[index];
	}
	return t;
}

template < typename T >
template < typename D >
Tensor< T > Tensor< T >::pow( const D &pow ) const
{
	TensorShape newShape( this->shape.dimsSize, this->shape.dim );
	Tensor t( std::move( newShape ) );
	ull index, k;
	uint j, p;
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

		t.r_data[i] = std::pow( this->r_data[index], pow );
	}
	return t;
}

template < typename T >
Tensor< T > Tensor< T >::operator>( const T &scalar ) const
{
	TENSOR_OPER_SCALER_LBE_LOGIC( > )
}
template < typename U, typename T >
void dfs_equal( Tensor< U > *result, const Tensor< T > *bigger_p, const Tensor< T > *smaller_p, const uint dim, const uint dim_index, const ull r_r_data_index, const ull b_r_data_index, const ull s_r_data_index, const uint dif )
{
	if ( dim == dim_index )
		result->r_data[r_r_data_index] = bigger_p->r_data[b_r_data_index] == smaller_p->r_data[s_r_data_index];
	else if ( dim_index < dif )
		for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
			dfs_equal( result, bigger_p, smaller_p, dim, dim_index + 1,
			           r_r_data_index + i * ( result->shape.stride[dim_index] ),
			           b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
			           s_r_data_index,
			           dif );
	else
	{
		if ( result->shape.dimsSize[dim_index] == 1 )
			dfs_equal( result, bigger_p, smaller_p, dim, dim_index + 1, r_r_data_index, b_r_data_index, s_r_data_index, dif );
		else if ( bigger_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_equal( result, bigger_p, smaller_p, dim, dim_index + 1,
				           r_r_data_index + i * ( result->shape.stride[dim_index] ),
				           b_r_data_index,
				           s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				           dif );
		else if ( smaller_p->shape.dimsSize[dim_index] == 1 )
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_equal( result, bigger_p, smaller_p, dim, dim_index + 1,
				           r_r_data_index + i * ( result->shape.stride[dim_index] ),
				           b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				           s_r_data_index,
				           dif );
		else
			for ( uint i = 0; i < result->shape.dimsSize[dim_index]; ++i )
				dfs_equal( result, bigger_p, smaller_p, dim, dim_index + 1,
				           r_r_data_index + i * ( result->shape.stride[dim_index] ),
				           b_r_data_index + i * ( bigger_p->shape.stride[dim_index] ),
				           s_r_data_index + i * ( smaller_p->shape.stride[dim_index - dif] ),
				           dif );
	}
}
template < typename T >
Tensor< ull > Tensor< T >::equal( const Tensor &other ) const
{
	const Tensor< T > *bigger_p, *smaller_p;
	uint result_dim;
	if ( this->shape.dim > other.shape.dim )
	{
		bigger_p = this;
		smaller_p = &other;
		result_dim = this->shape.dim;
	}
	else
	{
		bigger_p = &other;
		smaller_p = this;
		result_dim = other.shape.dim;
	}
	uint *result_dimsSize = new uint[result_dim], i = 0, dif = bigger_p->shape.dim - smaller_p->shape.dim, bigger_dimSize;
	for ( ; i < dif; ++i )
		result_dimsSize[i] = bigger_p->shape.dimsSize[i];
	for ( ; i < result_dim; ++i )
	{
		bigger_dimSize = smaller_p->shape.dimsSize[i - dif];
		if ( bigger_dimSize < bigger_p->shape.dimsSize[i] )
		{
			if ( bigger_dimSize != 1 )
			{
				throw std::runtime_error( "tensor oper broadcast: dim do not match" );
			}
			bigger_dimSize = bigger_p->shape.dimsSize[i];
		}
		else if ( bigger_dimSize > bigger_p->shape.dimsSize[i] && bigger_p->shape.dimsSize[i] != 1 )
		{
			cout << i << ":" << bigger_p->shape;
			cout << i << ":" << smaller_p->shape;
			throw std::runtime_error( "tensor oper broadcast: dim do not match1" );
		}
		result_dimsSize[i] = bigger_dimSize;
	}
	Tensor< ull > result = Tensor< ull >( TensorShape( result_dimsSize, result_dim ) );
	delete[] result_dimsSize;
	dfs_equal( &result, bigger_p, smaller_p, result_dim, 0, 0, bigger_p->shape.offset, smaller_p->shape.offset, dif );

	return result;
}