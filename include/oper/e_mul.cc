#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct E_Mul : public Oper< T >
{
	TensorHolder< T > *a, *b;
	bool holdA, holdB;
	E_Mul( TensorHolder< T > *a, TensorHolder< T > *b, bool holdA = false, bool holdB = false ) : a( a ), b( b ), holdA( holdA ), holdB( holdB ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		b->cal();
		ans.set( a->tensor.eMul( ( b->tensor ) ) );
#ifdef TENSOR_DEBUG
		cout << *a << "e*" << *b << "->";
		cout << ans << endl;
#endif
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			a->gradHolder->operator+=( TensorHolder< T >::eMul( *( ans.gradHolder ), *b ) );
			if ( a->creator && a->gradCleared )
				a->creator->buildGrad( *a );
			a->gradCleared = false;
		}
		if ( b->needGrad )
		{
			b->gradHolder->operator+=( TensorHolder< T >::eMul( *( ans.gradHolder ), *b ) );

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
	~E_Mul()
	{
		if ( holdA )
			delete a;
		if ( holdB )
			delete b;
	}
};