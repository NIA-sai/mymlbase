#include "base.hpp"
#include <stdexcept>
#include <utility>
struct ArrInterface
{
};
template < class T >
struct Arr : ArrInterface
{
	T *data = nullptr;
	ull size = 0;
	ull capacity = 0;
	Arr( bool, T *data, ull size, ull capacity ) : data( data ), size( size ), capacity( capacity ) {}
	Arr( ull cap = 31 ) : data( new T[cap] ), size( 0 ), capacity( cap ) {}
	Arr( const T *data, ull size ) : data( new T[size] ), size( size ), capacity( size )
	{
		for ( ull i = 0; i < size; ++i )
			this->data[i] = data[i];
	}
	Arr( const Arr &other ) : data( new T[other.size] ), size( other.size ), capacity( other.capacity )
	{
		for ( ull i = 0; i < size; ++i )
			this->data[i] = other.data[i];
	}
	Arr( Arr &&other ) : data( other.data ), size( other.size ), capacity( other.capacity )
	{
		other.data = nullptr;
		other.size = 0;
	}
	Arr &pushBack( const T &value )
	{
		if ( size == capacity )
			this->doubleCap();
		data[size++] = value;
		return *this;
	}
	Arr &pushBack( T &&value )
	{
		if ( size == capacity )
			this->doubleCap();
		data[size++] = std::move( value );
		// std::cout << "value: " << data[size - 1] << std::endl;

		return *this;
	}
	Arr &doubleCap()
	{
		capacity *= 2;
		T *newData = new T[capacity];
		for ( ull i = 0; i < size; ++i )
			newData[i] = std::move( data[i] );
		delete[] data;
		data = newData;
		return *this;
	}

	T &operator[]( ull i ) const
	{
		if ( i >= capacity )
			throw std::runtime_error( "Arr:Index out of range" );
		return data[i];
	}
	Arr &replace( ull i, const T &value )
	{
		if ( i <= size )
			data[i] = value;
		return *this;
	}
	const T Avg() { return T(); };
	Arr &operator=( Arr &&other )
	{
		if ( this != &other )
		{
			delete[] data;
			data = std::move( other.data );
			size = other.size;
			capacity = other.capacity;
			other.data = nullptr;
			other.size = 0;
		}
		return *this;
	}
	Arr &operator=( const Arr &other )
	{
		if ( this != &other )
		{
			delete[] data;
			data = new T[other.size];
			for ( ull i = 0; i < other.size; ++i )
				data[i] = other.data[i];
		}
		return *this;
	}
	~Arr() { delete[] data; };
};
#define 神晶构 zsgbp____arr_struct
#define 神晶类 zsgbp____arr_type
#define 神晶   zsgbp____arr

template < class T, uint dim >
struct 神晶构
{
	using type = Arr< typename 神晶构< T, dim - 1 >::type >;
};
template < class T >
struct 神晶构< T, 0 >
{
	using type = T;
};

template < class T, uint dim = 3 >
using 神晶 = typename 神晶构< T, dim >::type;