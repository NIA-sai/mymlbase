#include "tensor/tensor.cc"
#include <cmath>
using std::cout;
using std::endl;
// 未归一
double ball_guess_parzen_pos( uint dimSize, double h, Tensor< double > &x, Tensor< double > *claz_sample, uint sample_cnt )
{
	double pos = 0, dis;
	h *= 2 * h;
	for ( uint i = 0; i < sample_cnt; ++i )
	{
		dis = 0;
		for ( uint j = 0; j < dimSize; ++j )
			dis -= pow( ( x[j].oneValue() - claz_sample[i][j].oneValue() ), 2 );
		pos += exp( dis / h );
	}
	return pos;
}
int main()
{
	double claz[3][30] = {
	    { -1.63, 1.31, 0.28,
	      -0.78, 0.58, 1.52,
	      5.73, 0.21, -0.81,
	      -4.32, 1.18, -0.44,
	      -6.2, 0.82, 0.07,
	      2.77, 3.16, 0.65,
	      4.38, 2.42, 2.20,
	      6.21, 2.01, 0.91,
	      -0.96, 1.93, 1.54,
	      -0.19, 1.94, -0.26 },
	    { 1.03, -2.18, 0.08,
	      1.96, 1.27, 0.45,
	      3.12, 0.13, 0.16,
	      1.23, -0.21, -0.11,
	      1.39, -1.44, -0.19,
	      0.82, 0.34, -0.16,
	      0.94, -1.38, -0.21,
	      1.28, -0.12, 0.17,
	      2.31, 0.011, 0.14,
	      1.94, 0.26, 0.08 },
	    { 0.68, 0.14, 2.17,
	      1.41, 0.92, 0.13,
	      1.22, 0.69, 0.99,
	      2.46, -0.38, 1.45,
	      0.85, 0.87, 0.79,
	      2.51, 1.35, 3.22,
	      0.60, 1.31, 2.44,
	      0.64, 0.97, 2.19,
	      1.36, 0.99, 0.58,
	      0.66, 0.88, 0.51 } };

	double test[3][3] = {
	    { 0.5, 1.0, 0.0 },
	    { 0.31, 1.51, -0.50 },
	    { -0.3, 0.44, -0.1 } };
	Tensor< double > claz_sample[3][10];
	for ( uint i = 0; i < 3; ++i )
		for ( uint j = 0; j < 10; ++j )
			claz_sample[i][j] = Tensor< double >( { 3 }, claz[i] + j * 3 );
	Tensor< double > test_sample[3];
	for ( uint i = 0; i < 3; ++i )
		test_sample[i] = Tensor< double >( { 3 }, test[i] );
	double h = 1, pos[3];
	for ( uint i = 0; i < 3; ++i )
	{
		cout << "test<<" << i + 1 << ":" << test_sample[i];
		for ( uint j = 0; j < 3; ++j )
			cout << "pos_class" << j + 1 << ": " << ball_guess_parzen_pos( 3, h, test_sample[i], claz_sample[j], 10 ) << endl;
	}
}