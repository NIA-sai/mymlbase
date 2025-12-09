#pragma once
#include <iostream>
template < typename T >
struct Optional
{
	using OptionalType = T;
	bool has_value;
	T value;
	Optional() : has_value( false ) {}
	constexpr Optional( T v ) : has_value( true ), value( v ) {}
	bool hasValue() const { return has_value; }
	friend std::ostream &operator<<( std::ostream &os, const Optional &opt )
	{
		if ( opt.has_value )
			os << opt.value;
		else
			os << "Optional Null";
		return os;
	}
};

template < typename T, typename = void >
struct NotOptionalStruct
{
	using Value = void;
};
template < typename T >
struct NotOptionalStruct< T, VoidType< typename T::OptionalType > >
{
};
