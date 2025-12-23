#include "../2/N_dis_utils.cpp"
#include "tensor.hpp"
// #include <matioCpp/matioCpp.h>
// void load_data( const std::string &filename, Tensor< double > *&X, Tensor< double > *&y )
// {
// 	matioCpp::File raw_data( filename );

// 	// std::cout << int( raw_data.read( "y" ).valueType() ) << std::endl;

// 	auto raw_train_X = raw_data.read( "X" ).asMultiDimensionalArray< double >();
// 	auto raw_train_y = raw_data.read( "y" ).asVector< double >();
// 	X = new Tensor< double >( (uint[]){ raw_train_X.dimensions()[0], raw_train_X.dimensions()[1] } );
// 	y = new Tensor< double >( (uint[]){ raw_train_y.size() } );
// 	for ( ull i = 0; i < raw_train_y.size(); ++i )
// 	{
// 		X->data( i, 0 ) = raw_train_X[{ i, 0 }];
// 		X->data( i, 1 ) = raw_train_X[{ i, 1 }];
// 		y->data( i ) = raw_train_y[i];
// 	}
// 	raw_data.close();
// }
int main()
{
	// Tensor< double > *train_X, *train_y;
	// load_data( "./ex5data1.mat", train_X, train_y );
	// train_X->toCSV( "d/train_X.csv" );
	// train_y->toCSV( "d/train_y.csv" );

	// Tensor< double > *test_X, *test_y;
	// load_data( "./ex5data2.mat", test_X, test_y );
	// test_X->toCSV( "d/test_X.csv" );
	// test_y->toCSV( "d/test_y.csv" );

	// auto test_y = Tensor< double >::FromCSV( "test_y_rl.csv" );
	// test_y.reshape( { (uint)test_y.size, 1U } );
	// test_y.toCSV( "test_y_rl1.csv" );
	auto train_X = Tensor< double >::FromCSV( "train_X.csv" );
	// auto train_y = Tensor< double >::FromCSV( "train_y_rl1.csv" );
	// auto tt = Tensor< double >( { 50U, 3U } );
	// tt( 0,0,1,1 ) = train_y;
	// tt( 1, 2 ,1,1) = train_X;
	// tt.toCSV( "tt.csv" );

	// ( train_X/train_X.nPow( 2 ).sum( 1 ).pow( 0.5 ) ).toCSV( "train_X_norm.csv" );
	
	( ( train_X - train_X.min( 0 ) ) / ( train_X.max( 0 ) - train_X.min( 0 ) ) ).toCSV( "train_X_div.csv" );
}
/*
 g++ -I "d:\Code\Cpp\Projects\ml\mymlbase/include/" -I "d:\Code\Cpp\Projects\ml\mymlbase/vcpkg_installed/x64-mingw-dynamic/include/" -L "d:\Code\Cpp\Projects\ml\mymlbase/vcpkg_installed/x64-mingw-dynamic/lib/"  -std=c++20  before.cpp -lmatioCpp  -o "../../output/before"
../../output/before.exe
 */