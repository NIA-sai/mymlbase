#include "arr.cc"
#include "base.hpp"
#include "bounded_partial_order.hpp"
#include "not.hpp"
#include "sto_func_map.cpp"
#include <iostream>
#include <string>
template < class T, class = void >
struct Column : public Arr< T >
{
	int a = 1;
	using Arr< T >::Arr;
	template < typename U >
	Column &catBack( const Column< U > &column )
	{
		for ( uint i = 0; i < column.size; ++i )
			this->pushBack( column[i] );
		return *this;
	}
};
template < typename T >
struct Column< T, VoidValue< BoundedPartialOrder::Static< T >::zero > > : public Arr< T >
{
	using Arr< T >::Arr;
	T min = BoundedPartialOrder::Static< T >::max, max = BoundedPartialOrder::Static< T >::min, avg = BoundedPartialOrder::Static< T >::zero, total = BoundedPartialOrder::Static< T >::zero;
	template < class U, class = NotArray< const char, U > >
	Column &pushBack( U &value )
	{
		return pushBack( static_cast< T >( value ) );
	}
	Column &pushBack( const std::string &value )
	{
		return pushBack( sto< T >( value ) );
	}

	Column &pushBack( const T &value )
	{
		if ( value < min )
			min = value;
		if ( value > max )
			max = value;
		total += value;
		avg = BoundedPartialOrder::Static< T >::zero;
		Arr< T >::pushBack( value );
		return *this;
	}

	template < typename U >
	Column &catBack( const Column< U > &column )
	{
		for ( uint i = 0; i < column.size; ++i )
			this->pushBack( column[i] );
		return *this;
	}

	inline const T Avg()
	{
		return avg ? avg : avg = total / this->size;
	}
	Column &replace( uint i, const T &value )
	{
		if ( i > this->size )
			return *this;
		if ( value < min )
			min = value;
		if ( value > max )
			max = value;
		total -= this->data[i];
		total += value;
		avg = BoundedPartialOrder::Static< T >::zero;
		Arr< T >::replace( i, value );
		return *this;
	}
};
int main( int argc, char const *argv[] )
{
	return 0;
}
