#pragma once
#include "base.hpp"
#define BPO_Default_Explicit_InstantiationStatic( T )                          \
	template <>                                                    \
	struct Static< T >      \
	{                                                              \
		static constexpr T min = MIN_##T, max = MAX_##T, zero = 0; \
	}

namespace BoundedPartialOrder
{
	template < class T >
	struct Static
	{
	};

	// template <>
	// struct Static< uint >
	// {
	// 	static constexpr uint min = 0, max = MAX_uint, zero = 0;
	// };
	BPO_Default_Explicit_InstantiationStatic( uint );
	BPO_Default_Explicit_InstantiationStatic( int );
	BPO_Default_Explicit_InstantiationStatic( ll );
	BPO_Default_Explicit_InstantiationStatic( ull );
	BPO_Default_Explicit_InstantiationStatic( double );
	BPO_Default_Explicit_InstantiationStatic( ldouble );
	BPO_Default_Explicit_InstantiationStatic( float );

}
