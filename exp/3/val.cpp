#define TENSOR_DEBU
#include "arr.cc"
#include "tensor.hpp"
#include "tensor/initializer.cpp"
#include "utils/timer.cpp"
using std::string;
// todo batch
uint load_data( string filename, Tensor< double > *&data, Tensor< double > *&label, uint batch_size = 250, uint data_size = 28 * 28, uint label_size = 1, bool has_header = true, uint claz_cnt = 10 )
{

	Tensor< double > raw_tensor = Tensor< double >::FromCSV( filename, has_header );
	const uint batch_cnt = ( raw_tensor.shape.dimsSize[0] - 1 ) / batch_size + 1;
	data = new Tensor< double >[batch_cnt];
	label = new Tensor< double >[batch_cnt];
	for ( uint i = 0; i < batch_cnt; ++i )
	{
		data[i] = raw_tensor( i * batch_size, ( i + 1 ) * batch_size - 1, 1 )( label_size, -1, 1, 1 ).copy();
		label[i] = raw_tensor( i * batch_size, ( i + 1 ) * batch_size - 1, 1 )( 0, label_size - 1, 1, 1 ).oneHot( claz_cnt );
	}
	return batch_cnt;
}

struct BP_MLP
{
	Tensor< double > w1, b1, w2, b2, x, y;
	TensorHolder< double > w1h, b1h, w2h, b2h, xh, yh,
	    t1, t2, t3, t4, t5,
	    t6,
	    pre_y, t7, t8;
	TensorHolder< double > loss;
	const uint size1, size2, claz_size;
	double learn_rate = 0.01;
	BP_MLP( uint size1, uint size2, uint claz_size = 10, double learn_rate = 0.01 )
	    : size1( size1 ), size2( size2 ), claz_size( claz_size ), learn_rate( learn_rate ),
	      w1h( w1 ), b1h( b1 ), w2h( w2 ), b2h( b2 ), xh( x, false ), yh( y, false ),
	      t1( xh * w1h ),
	      t2( t1 + b1h ),
	      t3( t2.ReLU() ),
	      t4( t3 * w2h ),
	      t5( t4 + b2h ),
	      t6( Exp( t5 - t5.max( 1 ) ) ),
	      pre_y( t6 / t6.sum( 1 ) ),
	      // pre_y( ( ( xh * w1h + b1h ).ReLU() * w2h + b2h ).ReLU() ),
	      t7( Ln( pre_y ) ),
	      t8( TensorHolder< double >::eMul( t7, yh ).sum() ),
	      loss( -t8 )
	{
	}
	BP_MLP( const BP_MLP & ) = delete;
	BP_MLP &operator=( const BP_MLP & ) = delete;
	~BP_MLP() {}
	template < typename Initializer1, typename Initializer2, typename Initializer3, typename Initializer4 >
	bool init( Initializer1 &&init_w1, Initializer2 &&init_b1, Initializer3 &&init_w2, Initializer4 &&init_b2 )
	{
		w1 = init_w1( { { this->size1, this->size2 } } );
		b1 = init_b1( { { 1, this->size2 } } );
		w2 = init_w2( { { this->size2, this->claz_size } } );
		b2 = init_b2( { { 1, this->claz_size } } );
		return true;
	}
	double train( const Tensor< double > &x, const Tensor< double > &y )
	{
		double learn_rate = this->learn_rate / x.shape.dimsSize[0];
		this->x = x.copy();
		this->y = y;
		loss.force_cal();

		loss.buildGradAsMain();
		t8.grad().cal();
		t7.grad().cal();
		pre_y.grad().cal();
		t6.grad().cal();
		t5.grad().cal();
		t4.grad().cal();
		t3.grad().cal();
		t2.grad().cal();
		t1.grad().cal();



		w1h.grad().cal();
		b1h.grad().cal();
		w2h.grad().cal();
		b2h.grad().cal();

		w1 -= w1h.grad().tensor * learn_rate;

		b1 -= ( b1h.grad().tensor.sum( 0 ) * learn_rate );

		w2 -= w2h.grad().tensor * learn_rate;

		b2 -= ( b2h.grad().tensor.sum( 0 ) * learn_rate );

		return loss.tensor.oneValue();
	}
	// 预测
	Tensor< double > operator()( const Tensor< double > &x )
	{
		this->x = x.copy();
		pre_y.force_cal();
		return pre_y.tensor;
	}
};

int main()
{
	const uint batch_size = 64, turns = 10;

	const double learn_rate = 0.6;




	BP_MLP mlp( 28 * 28, 50, 10, learn_rate );
	mlp.init( Tensor_Initializer::move< double >(
	              Tensor< double >::FromCSV( "w1.csv" ) ),
	          Tensor_Initializer::move< double >(
	              Tensor< double >::FromCSV( "b1.csv" ) ),
	          Tensor_Initializer::move< double >(
	              Tensor< double >::FromCSV( "w2.csv" ) ),
	          Tensor_Initializer::move< double >(
	              Tensor< double >::FromCSV( "b2.csv" ) ) );
	// mlp.init( Tensor_Initializer::random< double >( -1, 1 ), Tensor_Initializer::random< double >( -1, 1 ), Tensor_Initializer::random< double >( -1, 1 ), Tensor_Initializer::random< double >( -1, 1 ) );

	Tensor< double >
	    *test_data,
	    *test_label;
	double start = timer::now_ms();
	std::cout << "start load data 'mnist_test.csv'" << std::endl;
	uint test_size = load_data( "./mnist_test.csv", test_data, test_label, batch_size );
	double end = timer::now_ms();
	std::cout << "load data time: " << end - start << std::endl;
	uint right = 0;
	for ( uint i = 0; i < test_size; ++i )
	{
		if ( i == 0 ) test_data[i] /= test_data[i].sum( 1 );
		right += ( mlp( test_data[i] ).index_of_max( 1 ).equal( test_label[i].index_of_max( 1 ) ) ).sum().oneValue();
	}
	cout << (double)right / 10000;
	delete[] test_data;
	delete[] test_label;
	return 0;
}