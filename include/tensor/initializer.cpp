#pragma once
#include "tensor.cc"
namespace Tensor_Initializer
{
	template < class T = double >
	Tensor< T > zeros( const TensorShape &shape )
	{
		return Tensor< T >::All_of( shape, T( 0 ) );
	}
	template < class T = double >
	Tensor< T > ones( const TensorShape &shape )
	{
		return Tensor< T >::All_of( shape, T( 1 ) );
	}

	template < class T = double >
	auto all_of( const T &a )
	{
		return [a]( const TensorShape &shape ) -> Tensor< T >
		{
			return Tensor< T >::All_of( shape, T( a ) );
		};
	}
	template < class T = double >
	auto random( const T &a, const T &b )
	{
		return [a, b]( const TensorShape &shape ) -> Tensor< T >
		{
			Tensor< T > t( shape );
			for ( ull i = 0; i < t.size; ++i )
			{
				t.r_data[i] = T( (double)rand() / RAND_MAX * ( b - a ) + a );
			}
			return t;
		};
	};

	template < class T = double >
	auto move( Tensor< T > &&a )
	{
		return [t = std::move( a )]( const TensorShape &shape )mutable -> Tensor< T >
		{
			return std::move( t );
		};
	};

}
