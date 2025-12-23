#define INTEGRATE
#include "../2/KNN/classify.cpp"
#include "tensor/initializer.cpp"
// #include "../4/dt.cpp"
#include "../5/svm.cpp"
#include "tensor.hpp"
#include <unordered_map>
#include <utility>

template < typename Data, typename Label, typename Model >
struct ModelWrapper
{
	Model &model;
	ModelWrapper( Model &model ) : model( model ) {}
	ModelWrapper( const ModelWrapper & ) = delete;
	ModelWrapper &operator=( const ModelWrapper & ) = delete;
	~ModelWrapper() {}
	void train( const Data &data, const Label &label )
	{
		model.train( data, label );
	}
	Label operator()( const Data &data )
	{
		return model( data );
	}
};

template < typename Data >
struct DataWrapper
{
};

template < typename T >
struct DataWrapper< Tensor< T > >
{
	Tensor< T > data;
	DataWrapper( Tensor< T > &&data ) : data( std::forward< Tensor< T > >( data ) ) {}
	DataWrapper( const DataWrapper & ) = delete;
	DataWrapper &operator=( const DataWrapper & ) = delete;
	~DataWrapper() {}
	uint count()
	{
		return data.shape.dimsSize[0];
	}
	T oneValue()
	{
		return data[0];
	}
};


template < typename Data, typename Label, typename... Models >
struct Integrator : public ModelWrapper< Data, Label, Models >...
{
	Integrator( Models &...models ) : Integrator::ModelWrapper< Data, Label, Models >( models )... {}
	Integrator( const Integrator & ) = delete;
	Integrator &operator=( const Integrator & ) = delete;
	~Integrator() {}
	void train( const Data &data, const Label &label )
	{
		( Integrator::template ModelWrapper< Data, Label, Models >::train( data, label ), ... );
	}
	typename Label::DataType operator()( const Data &data )
	{
		std::unordered_map< typename Label::DataType, uint > count;

		( count[Integrator::template ModelWrapper< Data, Label, Models >::operator()( data ).oneValue()]++, ... );

		typename Label::DataType max_label;
		uint max_cnt = 0;
		for ( auto &i : count )
			if ( i.second > max_cnt )
				max_label = i.first, max_cnt = i.second;
		return max_label;
	}
};

struct BP_MLP
{
	Tensor< double > w1, b1, w2, b2, x, y;
	TensorHolder< double > w1h, b1h, w2h, b2h, xh, yh, t5, t6, pre_y;
	TensorHolder< double > loss;
	const uint size1, size2, claz_size, epochs;
	double learn_rate = 0.01;
	BP_MLP( uint size1, uint size2, uint claz_size, double learn_rate = 0.01, uint epochs = 10 )
	    : size1( size1 ), size2( size2 ), claz_size( claz_size ), learn_rate( learn_rate ), epochs( epochs ),
	      w1h( w1 ), b1h( b1 ), w2h( w2 ), b2h( b2 ), xh( x, false ), yh( y, false ),
	      t5( ( xh * w1h + b1h ).ReLU() * w2h + b2h ),
	      t6( Exp( t5 - t5.max( 1 ) ) ),
	      pre_y( t6 / t6.sum( 1 ) ),
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
	void train( const Tensor< double > &x, const Tensor< double > &y )
	{
		for ( uint i = 0; i < this->epochs; ++i )
			this->train_once( x, y );
	}

	void train_once( const Tensor< double > &x, const Tensor< double > &y )
	{
		double learn_rate = this->learn_rate / x.shape.dimsSize[0];
		this->x = x.copy();
		this->y = y;
		loss.force_cal();
		loss.buildGradAsMain();
		w1h.grad().cal();
		b1h.grad().cal();
		w2h.grad().cal();
		b2h.grad().cal();
		w1 -= w1h.grad().tensor * learn_rate;
		b1 -= b1h.grad().tensor.sum( 0 ) * ( learn_rate );
		w2 -= w2h.grad().tensor * learn_rate;
		b2 -= b2h.grad().tensor.sum( 0 ) * ( learn_rate );
	}
	Tensor< double > operator()( const Tensor< double > &x )
	{
		this->x = x.copy();
		pre_y.force_cal();
		return pre_y.tensor;
	}
};



int main()
{
	auto rdata = Tensor< double >::FromCSV( "diabetes.csv", true );
	auto data = rdata( 0U, rdata.shape.dimsSize[1] - 2, 1, 1 );
	auto label = rdata( rdata.shape.dimsSize[1] - 1, -1, 1, 1 );

	BP_MLP mlp( 8, 4, 2 );
	mlp.init( Tensor_Initializer::random< double >( -1, 1 ), Tensor_Initializer::random< double >( -1, 1 ), Tensor_Initializer::random< double >( -1, 1 ), Tensor_Initializer::random< double >( -1, 1 ) );

	SVM< SMO > svm( 0.2 );
	SVM< SMO, Gaussian_Kernel > gsvm( 0.2, Gaussian_Kernel( 0.4 ) );

	Integrator< Tensor< double >, Tensor< double >, BP_MLP, SVM< SMO >, SVM< SMO, Gaussian_Kernel > >
	    integrator( mlp, svm, gsvm );

	integrator.train( data, label );
	cout << integrator( data ) << endl;
}