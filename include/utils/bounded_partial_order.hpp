#pragma once
#include "../base.hpp"
#include "optional.cc"
#define BPO_Default_Explicit_InstantiationStatic( T )              \
	template <>                                                    \
	struct Static< T >                                             \
	{                                                              \
		using BPOType = T;                                         \
		static constexpr T min = MIN_##T, max = MAX_##T, zero = 0; \
	}
#define BPO_Default_Explicit_InstantiationStatic_Optional( T )     \
	template <>                                                    \
	struct Static< Optional< T > >                                 \
	{                                                              \
		using BPOType = T;                                         \
		static constexpr T min = MIN_##T, max = MAX_##T, zero = 0; \
	}

namespace BoundedPartialOrder
{
	template < class T >
	struct Static
	{
		using Not = void;  // not a bounded partial order
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
	BPO_Default_Explicit_InstantiationStatic_Optional( uint );
	BPO_Default_Explicit_InstantiationStatic_Optional( int );
	BPO_Default_Explicit_InstantiationStatic_Optional( ll );
	BPO_Default_Explicit_InstantiationStatic_Optional( ull );
	BPO_Default_Explicit_InstantiationStatic_Optional( double );
	BPO_Default_Explicit_InstantiationStatic_Optional( ldouble );
	BPO_Default_Explicit_InstantiationStatic_Optional( float );
}
