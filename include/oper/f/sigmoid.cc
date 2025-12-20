#pragma once
#include "../../tensor.hpp"
#include "../oper.hpp"
template < typename T >
struct Sigmoid : public Oper< T >
{
	TensorHolder< T > *a;
	bool holdA;
	Sigmoid( TensorHolder< T > *a, bool holdA = false ) : a( a ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		ans.set( a->tensor.sigmoid() );
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			a->gradHolder->operator+=( TensorHolder< T >::eMul( *( ans.gradHolder ), TensorHolder< T >::eMul( ans, T( 1 ) - ans ) ) );
			if ( a->creator && a->gradCleared )
				a->creator->buildGrad( *a );
			a->gradCleared = false;
		}
	}
	void clearGrad()
	{
		a->clearGrad();
	}
	void reset()
	{
		a->reset();
	}
	~Sigmoid()
	{
		if ( holdA )
			delete a;
	}
};