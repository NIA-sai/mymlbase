#pragma once

#include "tensor.cc"
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
Tensor< T > Tensor< T >::operator+( const Tensor< T > &other ) const
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
				throw std::runtime_error( "Dimensions do not match" );
			bigger_dimSize = bigger_p->shape.dimsSize[i];
		}
		else if ( bigger_dimSize > bigger_p->shape.dimsSize[i] && bigger_p->shape.dimsSize[i] != 1 )
			throw std::runtime_error( "Dimensions do not match" );
		result_dimsSize[i] = bigger_dimSize;
	}
	Tensor< T > result = Tensor< T >( TensorShape( result_dimsSize, result_dim ) );
	delete[] result_dimsSize;
	dfs_add( &result, bigger_p, smaller_p, result_dim, 0, 0, bigger_p->shape.offset, smaller_p->shape.offset, dif );
	return result;
}
template < typename T >
Tensor< T > Tensor< T >::operator-( const Tensor< T > &other ) const
{
}
template < typename T >
Tensor< T > Tensor< T >::operator*( const Tensor< T > &other ) const
{
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
}
template < typename T >
Tensor< T > Tensor< T >::operator/( const T &scalar ) const
{
}
template < typename T >
Tensor< T > &Tensor< T >::operator+=( const Tensor< T > &other )
{
}
template < typename T >
Tensor< T > &Tensor< T >::operator-=( const Tensor< T > &other )
{
}
template < typename T >
Tensor< T > &Tensor< T >::operator*=( const Tensor< T > &other )
{
}
template < typename T >
Tensor< T > &Tensor< T >::operator/=( const Tensor< T > &other )
{
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
}
template < typename T >
Tensor< T > &Tensor< T >::operator/=( const T &scalar )
{
}