#include "../arr.cc"
#include "../base.hpp"
#include "../utils/bounded_partial_order.hpp"
#include "../utils/not.hpp"
#include "../utils/sto_func_map.cpp"
#include <iostream>
#include <string>
template < class T, class = void >
struct Column : public Arr< T >
{
	using Arr< T >::Arr;

	template < class U, class = NotArray< const char, U >, class = Not< std::string, U >, class = Not< T, U > >
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
};

template <>
struct Column< std::string, void > : public Arr< std::string >
{
	using Arr< std::string >::Arr;

	template < class U, class = NotArray< const char, U >, class = Not< std::string, U > >
	Column &pushBack( U &value )
	{
		return pushBack( static_cast< std::string >( value ) );
	}


	Column &pushBack( const std::string &value )
	{
		Arr< std::string >::pushBack( value );
		return *this;
	}

	template < typename U >
	Column &catBack( const Column< U > &column )
	{
		for ( uint i = 0; i < column.size; ++i )
			this->pushBack( column[i] );
		return *this;
	}
};

template < class T >
struct Column< T, VoidType< typename T::OptionalType, typename BoundedPartialOrder::Static< T >::Not > > : public Arr< T >
{
	uint notNull = 0;
	using Arr< T >::Arr;
	template < class U, class = NotArray< const char, U >, class = Not< std::string, U >, class = Not< T, U > >
	Column &pushBack( U &value )
	{
		return pushBack( static_cast< T >( value ) );
	}
	Column &pushBack( const std::string &value )
	{
		if ( value.empty() )
			return pushBack( T() );
		return pushBack( T( sto< typename T::OptionalType >( value ) ) );
	}

	Column &pushBack( const T &value )
	{
		if ( value.hasValue() )
			++this->notNull;
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
	Column &replace( uint i, const T &value )
	{
		if ( !this->data[i].hasValue() && value.hasValue() )
			++notNull;
		else if ( this->data[i].hasValue() && !value.hasValue() )
			--notNull;
		Arr< T >::replace( i, value );
		return *this;
	}
};



template < typename T >
struct Column< T, VoidType< typename NotOptionalStruct< T >::value, typename BoundedPartialOrder::Static< T >::BPOType > > : public Arr< T >
{
	using Arr< T >::Arr;
	T min = BoundedPartialOrder::Static< T >::max, max = BoundedPartialOrder::Static< T >::min, avg = BoundedPartialOrder::Static< T >::zero, total = BoundedPartialOrder::Static< T >::zero;
	template < class U, class = NotArray< const char, U >, class = Not< std::string, U >, class = Not< T, U > >
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

template < typename T >
struct Column< T, VoidType< typename BoundedPartialOrder::Static< T >::BPOType, typename T::OptionalType > > : public Arr< T >
{
	uint notNull = 0;
	using Arr< T >::Arr;
	typename T::OptionalType min = BoundedPartialOrder::Static< T >::max, max = BoundedPartialOrder::Static< T >::min, avg = BoundedPartialOrder::Static< T >::zero, total = BoundedPartialOrder::Static< T >::zero;
	template < class U, class = NotArray< const char, U >, class = Not< std::string, U >, class = Not< T, U > >
	Column &pushBack( U &value )
	{
		return pushBack( static_cast< T >( value ) );
	}
	Column &pushBack( const std::string &value )
	{
		if ( value.empty() )
			return pushBack( T() );
		return pushBack( T( sto< typename T::OptionalType >( value ) ) );
	}

	Column &pushBack( const T &value )
	{
		if ( value.hasValue() )
		{
			if ( value.value < min )
				min = value.value;
			if ( value.value > max )
				max = value.value;
			total += value.value;
			++notNull;
		}
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

	inline const typename T::OptionalType Avg()
	{
		return avg ? avg : avg = total / this->notNull;
	}
	Column &replace( uint i, const T &value )
	{
		if ( i > this->size )
			return *this;
		if ( value.value < min )
			min = value.value;
		if ( value.value > max )
			max = value.value;
		total -= this->data[i];
		total += value.value;
		avg = BoundedPartialOrder::Static< T >::zero;
		if ( !this->data[i].hasValue() && value.hasValue() )
			++notNull;
		else if ( this->data[i].hasValue() && !value.hasValue() )
			--notNull;
		Arr< T >::replace( i, value );
		return *this;
	}
};
