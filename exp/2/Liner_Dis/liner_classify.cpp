#include "../N_dis_utils.cpp"
#include "base.hpp"
#include "tensor/tensor.cc"
#include <iostream>
#include <matioCpp/matioCpp.h>
using std::cout;
using std::endl;
int main()
{
	matioCpp::File raw_data( "./e2data1.mat" );
	// for ( auto &name : raw_data.variableNames() )
	// {
	// 	cout << name <<endl;
	// }
	auto raw_X = raw_data.read( "X" ).asMultiDimensionalArray< double >();
	auto raw_y = raw_data.read( "y" ).asVector< double >();
	Tensor< double > X( { raw_X.dimensions()[0], raw_X.dimensions()[1] } );
	Tensor< double > y( { raw_y.size() } );
	for ( ull i = 0; i < raw_y.size(); ++i )
	{
		X.data( i, 0 ) = raw_X[{ i, 0 }];
		X.data( i, 1 ) = raw_X[{ i, 1 }];
		y.data( i ) = raw_y[i];
	}
	raw_data.close();

	// whatever 以下特化了
	uint claz0_cnt = 0, claz1_cnt = 0;
	for ( ull i = 0; i < y.size; ++i )
	{
		if ( y.data( i ) <= TENSOR_SCALAR_ZERO )
			++claz0_cnt;
	}
	Tensor< double > claz0X( { claz0_cnt, 2 } );
	Tensor< double > claz1X( { y.size - claz0_cnt, 2 } );
	claz0_cnt = 0;
	for ( ull i = 0; i < y.size; ++i )
	{
		if ( y.data( i ) <= TENSOR_SCALAR_ZERO )
			claz0X[claz0_cnt++] = X[i];
		else
			claz1X[claz1_cnt++] = X[i];
	}
	Tensor< double > mean0 = meanT2( claz0X );
	Tensor< double > mean1 = meanT2( claz1X );
	Tensor< double > cov0 = cov_unbiasT2( claz0X, mean0 );
	Tensor< double > cov1 = cov_unbiasT2( claz1X, mean1 );

	// fisher
	Tensor< double > Sw = cov0 + cov1, L( { 2, 2 } ), U( { 2, 2 } );
	Tensor< uint > ra = Tensor< double >::Max_I_I_Rearrange( Sw );
	Tensor< double >::LU_of( Sw, L, U, ra );
	Tensor< double > w = Tensor< double >::Solve_LU( L, U, mean0 - mean1, ra );


	Tensor< double > test( { 2 }, { 0.76615, 2.5899 } );
	cout << "test:" << test
	     << "claz0: " << ( w ^ mean0 ) << "\n claz1: " << ( w ^ mean1 )
	     << "\ntest: " << ( w ^ test ) << endl;

	return 0;
}
/*
 g++ -I "d:\Code\Cpp\Projects\ml\mymlbase/include/" -I "d:\Code\Cpp\Projects\ml\mymlbase/vcpkg_installed/x64-mingw-dynamic/include/" -L "d:\Code\Cpp\Projects\ml\mymlbase/vcpkg_installed/x64-mingw-dynamic/lib/"  -std=c++20  liner_classify.cpp -lmatioCpp  -o "../../../output/liner_classify"
../../../output/liner_classify.exe
 */