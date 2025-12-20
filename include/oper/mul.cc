#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
[[unrealized]] struct Mul : public Oper< T >
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

		);
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			a->gradHolder->operator+=( *( ans.gradHolder ) );
			if ( a->creator && a->gradCleared )
				a->creator->buildGrad( *a );
			a->gradCleared = false;
		}
		if ( b->needGrad )
		{
			b->gradHolder->operator+=( *( ans.gradHolder ) );
			if ( b->creator && b->gradCleared )
				b->creator->buildGrad( *b );
			b->gradCleared = false;
		}
	}
	void clearGrad()
	{
		a->clearGrad();
		b->clearGrad();
	}	void reset()
	{
		a->reset();
		b->reset();
	}
	~Mul()
	{
		if ( holdA )
			delete a;
		if ( holdB )
			delete b;
	}
};