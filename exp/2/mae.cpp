#include "N_dis_utils.cpp"
#include "tensor/tensor.cc"
using std::cout;
using std::endl;
double mean( Tensor< double > &t )
{
	double sum = 0;
	for ( uint i = 0; i < t.shape.size; ++i )
		sum += t.data( i );
	return sum / t.shape.size;
}
double cov( Tensor< double > &t, double mean )
{
	double sum = 0, tmp;
	for ( uint i = 0; i < t.shape.size; ++i )
	{
		tmp = t.data( i ) - mean;
		sum += tmp * tmp;
	}
	return sum / t.shape.size;
}



int main()
{
	double data1[] = { 0.76, -2.1, -1.9,
	                   -1, -2.6, 0.87,
	                   -5.3, -0.15, -1.6,
	                   0.89, -4.7, -0.029,
	                   1.9, 2.2, -0.23,
	                   -0.3, -0.87, 0.27,
	                   -0.087, 0.58, 0.42,
	                   -3.3, -3.4, -0.2,
	                   -0.32, 1.7, 1.3,
	                   0.71, 0.23, 0.39 };
	double data2[] = { 0.49, 0.0012, -0.065,
	                   0.054, -0.063, -0.12,
	                   0.47, 0.034, -0.35,
	                   0.69, 0.1, 0.17,
	                   0.55, -0.18, -0.011,
	                   0.61, 0.12, -0.27,
	                   0.58, 0.089, -0.4,
	                   0.27, -0.04, -0.31,
	                   0.055, -0.035, 0.38,
	                   0.53, 0.011, -0.15 };
	Tensor< double > claz[]{
	    Tensor< double >( { 10, 3 }, data1 ), Tensor< double >( { 10, 3 }, data2 ) };
	cout << "(1):\n";
	for ( uint i = 0; i < 3; ++i )
	{
		double mean1 = mean( claz[0].transpose()[i] );
		double mean2 = mean( claz[1].transpose()[i] );
		double var1 = cov( claz[0].transpose()[i], mean1 );
		double var2 = cov( claz[1].transpose()[i], mean2 );
		cout << i + 1
		     << ":\nclass 1:\nmean:" << mean1 << "var:" << var1
		     << "class 2:\nmean:" << mean2 << "var:" << var2 << endl;
	}
	cout << "\n(2):\n";
	Tensor< double > mean1 = meanT2( claz[0]( 0, 1, 1, 2 ) );
	Tensor< double > mean2 = meanT2( claz[1]( 0, 1, 1, 2 ) );
	Tensor< double > cov1 = covT2( claz[0]( 0, 1, 1, 2 ), mean1 );
	Tensor< double > cov2 = covT2( claz[1]( 0, 1, 1, 2 ), mean2 );
	cout << "1:\nclass 1:\nmean:" << mean1 << "cov:" << cov1
	     << "\nclass 2:\nmean:" << mean2 << "cov:" << cov2 << endl;

	mean1 = meanT2( claz[0]( 0, 2, 2, 2 ) );
	mean2 = meanT2( claz[1]( 0, 2, 2, 2 ) );
	cov1 = covT2( claz[0]( 0, 2, 2, 2 ), mean1 );
	cov2 = covT2( claz[1]( 0, 2, 2, 2 ), mean2 );
	cout << "2:\nclass 1:\nmean:" << mean1 << "cov:" << cov1
	     << "\nclass 2:\nmean:" << mean2 << "cov:" << cov2 << endl;

	mean1 = meanT2( claz[0]( 1, 2, 1, 2 ) );
	mean2 = meanT2( claz[1]( 1, 2, 1, 2 ) );
	cov1 = covT2( claz[0]( 1, 2, 1, 2 ), mean1 );
	cov2 = covT2( claz[1]( 1, 2, 1, 2 ), mean2 );
	cout << "3:\nclass 1:\nmean:" << mean1 << "cov:" << cov1
	     << "\nclass 2:\nmean:" << mean2 << "cov:" << cov2 << endl;

	mean1 = meanT2( claz[0] );
	mean2 = meanT2( claz[1] );
	cov1 = covT2( claz[0], mean1 );
	cov2 = covT2( claz[1], mean2 );
	cout << "\n(3):\nclass 1:\nmean:" << mean1 << "cov:" << cov1
	     << "\nclass 2:\nmean:" << mean2 << "cov:" << cov2 << endl;

	return 0;
}