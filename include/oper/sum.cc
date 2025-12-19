#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Sum : public Oper< T >
{
	TensorHolder< T > *a;
	bool holdA;
	Sum( TensorHolder< T > *a, bool holdA = false ) : a( a ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		ans.set( a->tensor.sum() );
	}

	void buildGrad( const TensorHolder< T > &ans )
	{
		a->gradHolder->operator+=( *( ans.gradHolder ) );
		if ( a->creator && a->gradCleared )
			a->creator->buildGrad( *a );
		a->gradCleared = false;
	}
	void clearGrad()
	{
		a->clearGrad();
	}
	~Sum()
	{
		if ( holdA )
			delete a;
	}
};