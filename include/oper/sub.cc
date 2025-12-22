#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Sub : public Oper< T >
{
	TensorHolder< T > *a, *b;
	bool holdA, holdB;
	Sub( TensorHolder< T > *a, TensorHolder< T > *b, bool holdA = false, bool holdB = false ) : a( a ), b( b ), holdA( holdA ), holdB( holdB )
	{
	}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		b->cal();
#ifdef TENSOR_DEBUG1
		cout << *a << "-" << *b << "->";
#endif
		ans.set( a->tensor - b->tensor );
#ifdef TENSOR_DEBUG1

		cout << ans << endl;
#endif
	}
	// f(c)=x
	// c=a+b
	// ans.grad = dx/dc
	// a.grad =dx/da  += dx/dc * 1
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
			b->gradHolder->operator-=( *( ans.gradHolder ) );
			if ( b->creator && b->gradCleared )
				b->creator->buildGrad( *b );
			b->gradCleared = false;
		}
	}
	void clearGrad()
	{
		a->clearGrad();
		b->clearGrad();
	}
	void reset()
	{
		a->reset();
		b->reset();
	}
	~Sub()
	{
		if ( holdA )
			delete a;
		if ( holdB )
			delete b;
	}
};