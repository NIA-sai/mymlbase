#pragma once
#include <stdexcept>
#include <utility>
typedef unsigned int uint;
typedef unsigned long long ull;
template < class... T >
struct Tuple;
template < uint I, class T >
struct TupleEnd
{
	T value;
	TupleEnd() = default;
	template < class U >
	TupleEnd( U &&v ) : value( std::forward< U >( v ) ) {}
};

template < uint I, class This, class... T >
struct TupleCurve : TupleEnd< I, This >, TupleCurve< I + 1, T... >
{
	TupleCurve() = default;
	template < class U, class... V >
	TupleCurve( U &&u, V &&...v ) : TupleEnd< I, This >( std::forward< U >( u ) ), TupleCurve< I + 1, T... >( std::forward< V >( v )... ) {}
};
template < uint I, class T >
struct TupleCurve< I, T > : TupleEnd< I, T >
{
	TupleCurve() = default;
	template < class U >
	TupleCurve( U &&u ) : TupleEnd< I, T >( std::forward< U >( u ) ) {}
};

template < uint I, class Head, class... Tail >
struct ElementType
{
	using type = typename ElementType< I - 1, Tail... >::type;
};
template < class Head, class... Tail >
struct ElementType< 0, Head, Tail... >
{
	using type = Head;
};

// template < uint I, class... T >
// struct TupleGetter
// {
// 	using Type = typename ElementType< I, T... >::type;
// 	static Type &get( Tuple< T... > *t, uint index )
// 	{
// 		if ( index == I )
// 			return static_cast< TupleEnd< I, Type > & >( *t ).value;
// 		else
// 			return TupleGetter< I + 1, T... >::get( t, index );
// 	}
// };
// template < class... T >
// struct TupleGetter< sizeof...( T ) - 1, T... >
// {
// 	constexpr static uint I = sizeof...( T ) - 1;
// 	using Type = typename ElementType< I, T... >::type;
// 	static Type &get( Tuple< T... > *t, uint index )
// 	{
// 		if ( index != I )
// 			throw std::runtime_error( "Tuple index out of range" );
// 		else
// 			return static_cast< TupleEnd< I, Type > & >( *t ).value;
// 	}
// };

template < class... T >
struct Tuple : TupleCurve< 0, T... >
{
private:
	// void **value_p;
public:
	Tuple() = default;
	Tuple( const Tuple & ) = default;
	Tuple( Tuple && ) = default;
	Tuple(  T &...args ) : TupleCurve< 0, T... >( std::forward< T >( args )... )
	{
	}
	Tuple( T &&...args ) : TupleCurve< 0, T... >( std::forward< T >( args )... )
	{
		// value_p = new void *[sizeof...( T )];
		// for ( uint i = 0; i < sizeof...( T ); ++i )
		// 	value_p[i] = &args[i];
	}

	~Tuple() {
		// delete[] value_p;
	};
	template < uint I >
	auto &static_get()
	{
		using Type = typename ElementType< I, T... >::type;
		return ( static_cast< TupleEnd< I, Type > & >( *this ) ).value;
	}


	// template < uint I = 0 >
	// decltype( auto ) get( uint index )
	// {
	// 	if constexpr ( I >= sizeof...( T ) )
	// 		throw std::runtime_error( "Tuple index out of range" );
	// 	else
	// 	{
	// 		if ( index == I )
	// 			return static_get< I >();
	// 		else
	// 			return get< I + 1 >( index );
	// 	}
	// }

	template < uint I = 0 >
	void *get( uint index )
	{
		if constexpr ( I >= sizeof...( T ) )
		{
			throw std::runtime_error( "Tuple index out of range" );
			return nullptr;
		}
		else
		{
			if ( index == I )
				return static_cast< void * >( &static_get< I >() );
			else
				return get< I + 1 >( index );
		}
	}


	template < typename Func, ull... Is >
	void forEachImpl( Func &&f, std::index_sequence< Is... > )
	{
		( f( Is, static_get< Is >() ), ... );
	}

	template < typename Func >
	void forEach( Func &&f )
	{
		forEachImpl( std::forward< Func >( f ),
		             std::make_index_sequence< sizeof...( T ) >{} );
	}
	template < typename Func, ull... Is >
	auto &mapImpl( Func &&f, std::index_sequence< Is... > )
	{
		return *( new Tuple( f( std::get< Is >( *this ) )... ) );
	}

	template < typename Func >
	auto &map( Func &&f )
	{
		return mapImpl( std::forward< Func >( f ),
		                std::make_index_sequence< sizeof...( T ) >{} );
	}

	template < typename... Func, ull... Is >
	void applyImpl( std::index_sequence< Is... >, Func &&...f )
	{
		( f( static_get< Is >() ), ... );
	}
	template < typename... Func >
	void apply( Func &&...f )
	{
		applyImpl( std::make_index_sequence< sizeof...( Func ) >{}, std::forward< Func >( f )... );
	}




	// auto &get( uint index )
	// {
	// 	if ( index >= sizeof...( T ) )
	// 		throw std::runtime_error( "Tuple index out of range" );

	// 	return TupleGetter< 0, T... >::get( this, index );
	// }
};
