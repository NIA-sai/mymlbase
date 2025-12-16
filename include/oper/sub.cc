#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Sub : public Oper< T >
{
	TensorHolder< T > *a, *b;
	bool holdA, holdB;
	Sub( TensorHolder< T > *a, TensorHolder< T > *b, bool holdA = false, bool holdB = false ) : a( a ), b( b ), holdA( holdA ), holdB( holdB )
	{}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		b->cal();
		ans.set( a->tensor - b->tensor );
	}
	// f(c)=x
	// c=a+b
	// ans.grad = dx/dc
	// a.grad =dx/da  += dx/dc * 1
	void buildGrad( const TensorHolder< T > &ans )
	{
		a->gradHolder->operator+=( *( ans.gradHolder ) );
		b->gradHolder->operator-=( *( ans.gradHolder ) );
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
	~Sub()
	{
		if ( holdA )
			delete a;
		if ( holdB )
			delete b;
	}
};