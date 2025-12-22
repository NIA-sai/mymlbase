#pragma once
#include "../tensor.hpp"
#include "oper.hpp"
template < typename T >
struct MaxOD : public Oper< T >
{
	TensorHolder< T > *a;
	const uint dim_index;
	bool holdA;
	MaxOD( TensorHolder< T > *a, const uint &dim_index, bool holdA = false ) : a( a ), dim_index( dim_index ), holdA( holdA ) {}

	void exec( TensorHolder< T > &ans )
	{
		a->cal();
		ans.set( a->tensor.max( dim_index ) );
#ifdef TENSOR_DEBUG
		cout << *a << "->MaxOD";
		cout << ans << endl;
#endif
	}

	void buildGrad( TensorHolder< T > &ans )
	{
		if ( a->needGrad )
		{
			// todo:暂时应该不需要grad
			//  a->gradHolder->operator+=( TensorHolder< T >::eMul( *( ans.gradHolder ), a->tensor );
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
	~MaxOD()
	{
		if ( holdA )
			delete a;
	}
};