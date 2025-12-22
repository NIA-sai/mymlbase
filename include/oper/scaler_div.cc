#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Scaler_Div : public Oper< T >
{
	TensorHolder< T > *a;
	const T b;
	bool holdA;
	Scaler_Div( TensorHolder< T > *a, const T &b, bool holdA = false ) : a( a ), b( b ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		ans.set( b / a->tensor );

#ifdef TENSOR_DEBUG
		cout << *a << "*" << b << "->" << endl;
		cout << ans << endl;
#endif
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			a->gradHolder->operator+=( *ans.gradHolder * b );
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
	~Scaler_Div()
	{
		if ( holdA )
			delete a;
	}
};