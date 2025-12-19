#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Mul : public Oper< T >
{
	TensorHolder< T > *a, *b;
	const uint a_row_dim, a_col_dim, b_row_dim, b_col_dim;
	bool holdA, holdB;
	Mul( TensorHolder< T > *a, TensorHolder< T > *b, bool holdA = false, bool holdB = false, uint a_row_dim = 0, uint a_col_dim = 1, uint b_row_dim = 0, uint b_col_dim = 1 ) : a( a ), b( b ), holdA( holdA ), holdB( holdB ), a_row_dim( a_row_dim ), a_col_dim( a_col_dim ), b_row_dim( b_row_dim ), b_col_dim( b_col_dim ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		b->cal();
		ans.set(
		    Tensor<T>::Mul2D()

		);
	}

	void buildGrad( const TensorHolder< T > &ans )
	{
		a->gradHolder->operator+=( *( ans.gradHolder ) );
		b->gradHolder->operator+=( *( ans.gradHolder ) );
		if ( a->creator )
			a->creator->buildGrad( *a );
		if ( b->creator )
			b->creator->buildGrad( *b );
		a->gradCleared = false;
		b->gradCleared = false;
	}
	void clearGrad()
	{
		a->clearGrad();
		b->clearGrad();
	}
	~Mul()
	{
		if ( holdA )
			delete a;
		if ( holdB )
			delete b;
	}
};