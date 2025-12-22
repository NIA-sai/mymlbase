#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Negtive : public Oper< T >
{
	TensorHolder< T > *a;
	bool holdA;
	Negtive( TensorHolder< T > *a, bool holdA = false ) : a( a ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		ans.set( -a->tensor );

#ifdef TENSOR_DEBUG
		cout << "-" << *a << "->";
		cout << ans << endl;
#endif
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			a->gradHolder->operator+=( -( *ans.gradHolder ) );
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
	~Negtive()
	{
		if ( holdA )
			delete a;
	}
};