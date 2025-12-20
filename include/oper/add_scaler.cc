#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Add_Scaler : public Oper< T >
{
	TensorHolder< T > *a;
	const T b;
	bool holdA;
	Add_Scaler( TensorHolder< T > *a, const T &b, bool holdA = false ) : a( a ), b( b ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		ans.set( a->tensor + b );
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			a->gradHolder->operator+=( *ans.gradHolder );
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
	~Add_Scaler()
	{
		if ( holdA )
			delete a;
	}
};