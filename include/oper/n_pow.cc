#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct N_Pow : public Oper< T >
{
	TensorHolder< T > *a;
	const uint pow;
	bool holdA;
	N_Pow( TensorHolder< T > *a, uint pow, bool holdA = false ) : a( a ), pow( pow ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		ans.set( a->tensor.nPow( pow ) );
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			if ( pow > 1 )
				a->gradHolder->operator+=( TensorHolder< T >::eMul( *( ans.gradHolder ), ( a->n_pow( pow - 1 ) ) * T( pow ) ) );
			else
				a->gradHolder->operator+=( *( ans.gradHolder ) );
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
	~N_Pow()
	{
		if ( holdA )
			delete a;
	}
};