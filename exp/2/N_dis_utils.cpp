#pragma once
#include "tensor/tensor.cc"
#include <cmath>
Tensor< double > meanT2( Tensor< double > &t )
{
	Tensor< double > sum( { t.shape.dimsSize[1] } );
	for ( uint i = 0; i < t.shape.dimsSize[1]; ++i )
		sum.r_data[i] = 0;
	for ( uint i = 0; i < t.shape.dimsSize[1]; ++i )
		for ( uint j = 0; j < t.shape.dimsSize[0]; ++j )
			sum.r_data[i] += t.data( j, i );
	for ( uint i = 0; i < t.shape.dimsSize[1]; ++i )
		sum.r_data[i] /= t.shape.dimsSize[0];
	return sum;
}
Tensor< double > covT2( Tensor< double > &t, Tensor< double > mean )
{
	Tensor< double > sum( { t.shape.dimsSize[1], t.shape.dimsSize[1] } );
	for ( uint i = 0; i < t.shape.dimsSize[1]; ++i )
	{
		for ( uint j = i; j < t.shape.dimsSize[1]; ++j )
		{
			sum.data( i, j ) = 0;
			for ( uint k = 0; k < t.shape.dimsSize[0]; ++k )
				sum.data( i, j ) += ( t.data( k, i ) - mean.r_data[i] ) * ( t.data( k, j ) - mean.r_data[j] );
			sum.data( i, j ) /= t.shape.dimsSize[0];
			if ( i != j ) sum.data( j, i ) = sum.data( i, j );
		}
	}
	return sum;
}
Tensor< double > cov_unbiasT2( Tensor< double > &t, Tensor< double > mean )
{
	Tensor< double > sum( { t.shape.dimsSize[1], t.shape.dimsSize[1] } );
	for ( uint i = 0; i < t.shape.dimsSize[1]; ++i )
	{
		for ( uint j = i; j < t.shape.dimsSize[1]; ++j )
		{
			sum.data( i, j ) = 0;
			for ( uint k = 0; k < t.shape.dimsSize[0]; ++k )
				sum.data( i, j ) += ( t.data( k, i ) - mean.r_data[i] ) * ( t.data( k, j ) - mean.r_data[j] );
			sum.data( i, j ) /= ( t.shape.dimsSize[0] - 1 );
			if ( i != j ) sum.data( j, i ) = sum.data( i, j );
		}
	}
	return sum;
}
Tensor< double > stdT2( Tensor< double > &t, Tensor< double > mean )
{
	Tensor< double > sum( { t.shape.dimsSize[1] } );
	double tmp;
	for ( uint i = 0; i < t.shape.dimsSize[1]; ++i )
	{
		sum.data( i ) = 0;
		for ( uint j = 0; j < t.shape.dimsSize[0]; ++j )
		{
			tmp = ( t.data( j, i ) - mean.r_data[i] );
			sum.data( i ) += tmp * tmp;
		}
		sum.data( i ) /= t.shape.dimsSize[0];
		sum.data( i ) = sqrt( sum.data( i ) );
	}
	return sum;
}
Tensor< double > std_unbiasT2( Tensor< double > &t, Tensor< double > mean )
{
	Tensor< double > sum( { t.shape.dimsSize[1] } );
	double tmp;
	for ( uint i = 0; i < t.shape.dimsSize[1]; ++i )
	{
		sum.data( i ) = 0;
		for ( uint j = 0; j < t.shape.dimsSize[0]; ++j )
		{
			tmp = ( t.data( j, i ) - mean.r_data[i] );
			sum.data( i ) += tmp * tmp;
		}
		sum.data( i ) /= ( t.shape.dimsSize[0] - 1 );
		sum.data( i ) = sqrt( sum.data( i ) );
	}
	return sum;
}
void N_dis_normalizeT2( Tensor< double > &t )
{
	Tensor< double > mean = meanT2( t );
	Tensor< double > std = std_unbiasT2( t, mean );
	for ( uint i = 0; i < t.shape.dimsSize[0]; ++i )
		for ( uint j = 0; j < t.shape.dimsSize[1]; ++j )
			t.data( i, j ) = ( t.data( i, j ) - mean.r_data[j] ) / std.r_data[j];
}