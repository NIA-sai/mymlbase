#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Mul2D : public Oper< T >
{
	TensorHolder< T > *a, *b;
	bool holdA, holdB;
	Mul2D( TensorHolder< T > *a, TensorHolder< T > *b, bool holdA = false, bool holdB = false ) : a( a ), b( b ), holdA( holdA ), holdB( holdB ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		b->cal();
		ans.set( ( a->tensor ) * ( b->tensor ) );
	}

	void buildGrad( const TensorHolder< T > &ans )
	{
		a->gradHolder->operator+=( ans.gradHolder->operator*(  b->transpose() ) );
		b->gradHolder->operator+=( ans.gradHolder->operator*(  a->transpose() ) );
		// gradCleared为false时防止再递归扩散一遍
		if ( a->creator && a->gradCleared )
			a->creator->buildGrad( *a );
		if ( b->creator && b->gradCleared )
			b->creator->buildGrad( *b );
		a->gradCleared = false;
		b->gradCleared = false;
	}
	void clearGrad()
	{
		a->clearGrad();
		b->clearGrad();
	}
	~Mul2D()
	{
		if ( holdA )
			delete a;
		if ( holdB )
			delete b;
	}
};