#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Exp_OP : public Oper< T >
{
	TensorHolder< T > *a;
	bool holdA;
	Exp_OP( TensorHolder< T > *a, bool holdA = false ) : a( a ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
#ifdef TENSOR_DEBUG
		cout << "e^" << *a << "->";
#endif
		ans.set( Tensor< T >::Exp( a->tensor ) );
#ifdef TENSOR_DEBUG
		cout << ans << endl;
#endif
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			a->gradHolder->operator+=( TensorHolder< T >::eMul( *ans.gradHolder, ans ) );
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
	~Exp_OP()
	{
		if ( holdA )
			delete a;
	}
};