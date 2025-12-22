#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Ln_OP : public Oper< T >
{
	TensorHolder< T > *a;
	bool holdA;
	Ln_OP( TensorHolder< T > *a, bool holdA = false ) : a( a ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
#ifdef TENSOR_DEBUG
		cout << "ln(" << *a << ")->";
#endif
		ans.set( Tensor< T >::Ln( a->tensor ) );
#ifdef TENSOR_DEBUG
		cout << ans << endl;
#endif
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			a->gradHolder->operator+=( *ans.gradHolder / *a );
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
	~Ln_OP()
	{
		if ( holdA )
			delete a;
	}
};