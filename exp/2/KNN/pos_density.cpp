#include "tensor/tensor.cc"
#include <cmath>
#include <queue>
#include <utility>
using std::cout;
using std::endl;
constexpr double pi = 3.14159265358979323846;

auto cmp = []( const double &a, const double &b ) -> bool
{
	return a < b;
};
using pq = std::priority_queue< double, std::vector< double >, decltype( cmp ) >;
// 向量距离
double eu_dis2( uint dimSize, const Tensor< double > &a, const Tensor< double > &b )
{
	double sum = 0;
	for ( uint i = 0; i < dimSize; ++i )
		sum += pow( a.data( i ) - b.data( i ), 2 );
	return sqrt( sum );
}
double V( uint dimSize, double dis )
{
	switch ( dimSize )
	{
	case 1 :
		return 2 * dis;
	case 2 :
		return pi * dis * dis;
	case 3 :
		return 4 / 3 * pi * dis * dis * dis;
	default :
		return 1;
	}
}
double *knn_density( uint k, uint dimSize, const Tensor< double > *test, const Tensor< double > *sample, uint test_cnt, uint sample_cnt )
{
	double *pos_dens = new double[test_cnt];
	for ( uint t = 0; t < test_cnt; ++t )
	{
		pq kn;
		double dis;

		for ( uint j = 0; j < sample_cnt; ++j )
		{
			dis = eu_dis2( dimSize, test[t], sample[j] );
			if ( kn.size() < k )
				kn.push( dis );
			else if ( dis < kn.top() )
			{
				kn.pop();
				kn.push( dis );
			}
		}
		pos_dens[t] = double( k ) / ( sample_cnt * V( dimSize, kn.top() ) );
	}
	return pos_dens;
}

int main()
{
	double sample[3][30] = {
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



	// 1) 用样本作为测试点
	cout << "(1):\n";
	Tensor< double > exp1_sample_and_test[10];
	for ( uint i = 0; i < 10; ++i )
		exp1_sample_and_test[i] = Tensor< double >( { 1 }, sample[2] + i * 3 );

	uint n = 10;
	for ( uint i = 1; i < 6; i += 2 )
	{
		double *pos_dens = knn_density( i, 1, exp1_sample_and_test, exp1_sample_and_test, n, n );
		cout << "k = " << i << ":\n";
		for ( uint j = 0; j < n; ++j )
			cout << pos_dens[j] << ' ';
		cout << endl;
		delete[] pos_dens;
	}
	// (2) 同
	cout << "(2):\n";
	Tensor< double > exp2_sample_and_test[10];
	for ( uint i = 0; i < 10; ++i )
		exp2_sample_and_test[i] = Tensor< double >( { 2 }, sample[1] + i * 3 );
	for ( uint i = 1; i < 6; i += 2 )
	{
		double *pos_dens = knn_density( i, 2, exp2_sample_and_test, exp2_sample_and_test, n, n );
		cout << "k = " << i << ":\n";
		for ( uint j = 0; j < n; ++j )
			cout << pos_dens[j] << ' ';
		cout << endl;
		delete[] pos_dens;
	}


	cout << "(3):\n";
	double test[3][3] = {
	    { 0.5, 1.0, 0.0 },
	    { 0.31, 1.51, -0.50 },
	    { -0.3, 0.44, -0.1 } };
	Tensor< double > exp3_sample[3][10];
	Tensor< double > exp3_test[3];
	for ( uint i = 0; i < 3; ++i )
	{
		for ( uint j = 0; j < 10; ++j )
			exp3_sample[i][j] = Tensor< double >( { 3 }, sample[i] + j * 3 );
		exp3_test[i] = Tensor< double >( { 3 }, test[i] );
	}
	for ( uint i = 1; i < 6; i += 2 )
	{
		cout << "k = " << i << ":\n";
		for ( uint j = 0; j < 3; ++j )
		{
			cout << "class " << j + 1 << ":";
			double *pos_dens = knn_density( i, 3, exp3_test, exp3_sample[j], 3, 10 );
			for ( uint j = 0; j < 3; ++j )
				cout << pos_dens[j] << ' ';
			cout << endl;
			delete[] pos_dens;
		}
	}
}