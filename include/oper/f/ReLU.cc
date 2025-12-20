#pragma once
#include "../../tensor.hpp"
#include "../oper.hpp"
template < typename T >
struct ReLU_OP : public Oper< T >
{
	TensorHolder< T > *a;
	bool holdA;
	ReLU_OP( TensorHolder< T > *a, bool holdA = false ) : a( a ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		ans.set( a->tensor.ReLU() );
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			a->gradHolder->operator+=( TensorHolder< T >::eMul( *( ans.gradHolder ),
			                                                    // todo
			                                                    TensorHolder< T >( a->tensor > T( 0 ), false ) ) );
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
	~ReLU_OP()
	{
		if ( holdA )
			delete a;
	}
};