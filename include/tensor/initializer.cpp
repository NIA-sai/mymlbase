#pragma once
#include "tensor.cc"
namespace Tensor_Initializer
{
	template < class T = double >
	Tensor< T > zeros( const TensorShape &shape )
	{
		return Tensor< T >::All_of( shape, T( 1 ) / 2550 );
	}
}
