#define TENSOR_DEBU
// todo: 梯度裁剪
// 现在解决方案是直接逐条归一化数据，很不妥
#include "arr.cc"
#include "tensor.hpp"
#include "tensor/initializer.cpp"
#include "utils/timer.cpp"
using std::string;
uint load_data( string filename, Tensor< double > *&data, Tensor< double > *&label, uint batch_size = 250, uint data_size = 28 * 28, uint label_size = 1, bool has_header = true, uint claz_cnt = 10 )
{
	// Arr< string > *raw_data_arr = Arr< std::string >::FromCSV( filename );
	// Tensor< string > *raw_tensor = new Tensor< string >();
	// uint size = raw_data_arr->size / ( data_size + label_size ) - has_header;
	// raw_tensor->shape = TensorShape( (uint[]){ size, data_size + label_size } );
	// raw_tensor->r_data = raw_data_arr->data;
	// raw_data_arr->data = nullptr;
	// data = raw_tensor->operator()( has_header ? 1 : 0, -1 )( label_size, -1, 1, 1 ).to< double >();
	// label = raw_tensor->operator()( has_header ? 1 : 0, -1 )( 0, label_size, 1, 1 ).to< double >();
	// delete raw_tensor;
	// delete raw_data_arr;
	// return size;
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
	    // t1, t2, t3, t4,
	    t5,
	    t6,
	    pre_y
	    // , t7, t8
	    ;
	TensorHolder< double > loss;
	const uint size1, size2, claz_size;
	double learn_rate = 0.01;
	BP_MLP( uint size1, uint size2, uint claz_size = 10, double learn_rate = 0.01 )
	    : size1( size1 ), size2( size2 ), claz_size( claz_size ), learn_rate( learn_rate ),
	      w1h( w1 ), b1h( b1 ), w2h( w2 ), b2h( b2 ), xh( x, false ), yh( y, false ),
	      // t1( xh * w1h ),
	      // t2( t1 + b1h ),
	      // t3( t2.ReLU() ),
	      // t4( t3 * w2h ),
	      // t5( t4 + b2h ),
	      t5( ( xh * w1h + b1h ).ReLU() * w2h + b2h ),
	      t6( Exp( t5 - t5.max( 1 ) ) ),
	      pre_y( t6 / t6.sum( 1 ) ),
	      // pre_y( ( ( xh * w1h + b1h ).ReLU() * w2h + b2h ) ),
	      // t7( Ln( pre_y ) ),
	      // t8( TensorHolder< double >::eMul( t7, yh ).sum() ),
	      // loss( -t8 )
	      loss( -( TensorHolder< double >::eMul( Ln( pre_y ), yh ).sum() ) )
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
		// t8.grad().cal();
		// t7.grad().cal();
		// pre_y.grad().cal();
		// t6.grad().cal();
		// t5.grad().cal();
		// t4.grad().cal();
		// t3.grad().cal();
		// t2.grad().cal();
		// t1.grad().cal();



		w1h.grad().cal();
		b1h.grad().cal();
		w2h.grad().cal();
		b2h.grad().cal();
		// cout << pre_y.tensor[0] << endl;
		// cout << y[0] << endl;
		// cout << t7.tensor[0] << endl;

		// cout << pre_y.grad().tensor[0] << endl;
		// cout << t6.tensor[0] << endl;
		// cout << t6.grad().tensor[0] << endl;
		// cout << t5.tensor[0] << endl;
		// cout << t5.grad().tensor[0] << endl;
		// cout << t4.tensor << endl;
		// cout << t4.grad().tensor[0] << endl;
		// cout << w2[0]<< endl;
		// cout << t3.tensor[0] << endl;
		// cout << t3.grad().tensor << endl;
		// cout << t2.grad().tensor[0] << endl;
		// cout << t2.tensor[0] << endl;
		// cout << t1.grad().tensor[0] << endl;
		// cout << t1.tensor[0] << endl;
		// cout << w1h.grad().tensor[0] << endl;
		// cout << w1h.tensor[0] << endl;
		// cout << w1 << b1;
		// cout << t1 << t2 << t3 << t4 << t5 << t6<<"\nnn";
		// cout << "w2:" << w2;
		// cout << "t4.grad" << t4.grad();

		// cout << "pre_y.grad " << pre_y.grad();
		// cout << "pre_y " << pre_y;

		// cout
		// << "w2.grad():" << w2h.grad();
		// cout << "pre_y" << pre_y;
		// cout << pre_y.grad() << t6.grad() << t5.grad() << t4.grad();
		// cout<<"t4:"<<t4;
		// cout<<"t3:"<<t3;
		// cout<<"w2:"<<w2;
		// cout<< t3.grad() << t2.grad() << t1.grad() << "\n\n\naa";
		// cout << w1[0];
		w1 -= w1h.grad().tensor * learn_rate;
		// cout << w1h.grad().tensor[0];
		// cout << w1[0];
		// cout << b1h.grad().tensor.sum( 0 );
		// cout << b2h.grad().tensor.sum( 0 );
		b1 -= b1h.grad().tensor.sum( 0 ) * ( learn_rate / x.shape.dimsSize[0] );
		// cout << b1[0];
		// cout << w2[0];
		// cout<<w2h.grad().tensor[0];
		w2 -= w2h.grad().tensor * learn_rate;
		// cout << w2[0];
		// cout << b2[0];
		b2 -= b2h.grad().tensor.sum( 0 ) * ( learn_rate / x.shape.dimsSize[0] );
		// cout << b2[0];

		// cout << loss;
		// cout << pre_y.tensor[0];
		// loss.force_cal();
		// // cout << (x [1] * w1 + b1).ReLU() * w2 + b2;

		// cout << pre_y.tensor[0];
		// cout << y[0];
		// cout << loss;
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
#ifndef INTEGRATE

int main()
{
	const uint batch_size = 64, turns = 10;

	const double learn_rate = 0.6;
	double start = timer::now_ms();
	Tensor< double >
	    *train_data,
	    *train_label;
	std::cout << "start load data 'mnist_train.csv'" << std::endl;
	uint train_size = load_data( "./mnist_train.csv", train_data, train_label, batch_size );
	double end = timer::now_ms();
	std::cout << "load data time: " << end - start << std::endl;
	// start = timer::now_ms();
	// Tensor< double > *test_data, *test_label;
	// uint test_size = load_data( "./mnist_test.csv", test_data, test_label, batch_size );
	// end = timer::now_ms();
	// std::cout << "load data time: " << end - start << std::endl;



	BP_MLP mlp( 28 * 28, 50, 10, learn_rate );
	mlp.init( Tensor_Initializer::random< double >( -1, 1 ), Tensor_Initializer::random< double >( -1, 1 ), Tensor_Initializer::random< double >( -1, 1 ), Tensor_Initializer::random< double >( -1, 1 ) );
	double loss;
	for ( uint i = 0; i < turns; ++i )
	{
		cout << "statrt turn " << i << ":\n";
		loss = 0;
		start = timer::now_ms();
		for ( uint j = 0; j < train_size; ++j )
		{
			// cout << train_label[j].sum( 0 );
			if ( i == 0 ) train_data[j] /= train_data[j].sum( 1 );
			loss += mlp.train( train_data[j], train_label[j] );
		}
		end = timer::now_ms();
		cout << "train time: " << end - start << "ms\n loss: " << loss / batch_size / train_size << endl;
	}

	delete[] train_data;
	delete[] train_label;
	cout << mlp.w1.toCSV( "w1.csv" ) << endl
	     << mlp.b1.toCSV( "b1.csv" ) << endl
	     << mlp.w2.toCSV( "w2.csv" ) << endl
	     << mlp.b2.toCSV( "b2.csv" ) << endl;

	return 0;
}
#endif