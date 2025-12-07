#pragma once
#include "../base.hpp"
#define DEFAULT_Default_Explicit_InstantiationStatic( T ) \
	template <>                                   \
	struct Default< T >                           \
	{                                             \
		static constexpr T value = T();           \
	};
namespace Default
{
	template < class T >
	struct Default
	{
	};

	DEFAULT_Default_Explicit_InstantiationStatic( uint );
	DEFAULT_Default_Explicit_InstantiationStatic( int );
	DEFAULT_Default_Explicit_InstantiationStatic( ll );
	DEFAULT_Default_Explicit_InstantiationStatic( ull );
	DEFAULT_Default_Explicit_InstantiationStatic( double );
	DEFAULT_Default_Explicit_InstantiationStatic( ldouble );
	DEFAULT_Default_Explicit_InstantiationStatic( float );
}