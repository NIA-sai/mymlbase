#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct Transpose : public Oper< T >
{
	TensorHolder< T > *a;
	bool holdA;
	uint dim1_idx, dim2_idx;
	Transpose( TensorHolder< T > *a, uint dim1_idx = 0, uint dim2_idx = 1, bool holdA = false ) : a( a ), dim1_idx( dim1_idx ), dim2_idx( dim2_idx ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		ans.set( a->tensor.transpose( dim1_idx, dim2_idx ) );
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			a->gradHolder->operator+=( ans.gradHolder->transpose( dim1_idx, dim2_idx ) );
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
	~Transpose()
	{
		if ( holdA )
			delete a;
	}
};