#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct SumOD : public Oper< T >
{
	TensorHolder< T > *a;
	const uint dim_index;
	bool holdA;
	SumOD( TensorHolder< T > *a, const uint &dim_index, bool holdA = false ) : a( a ), dim_index( dim_index ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		ans.set( a->tensor.sum( dim_index ) );
#ifdef TENSOR_DEBUG
		cout << *a << "->SumOD";
		cout << ans << endl;
#endif
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			a->gradHolder->operator+=( ( ans.gradHolder->sum( dim_index ) ) );
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
	~SumOD()
	{
		if ( holdA )
			delete a;
	}
};