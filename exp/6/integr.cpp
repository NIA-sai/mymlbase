#define INTEGRATE
#include "../2/KNN/classify.cpp"
#include "../4/dt.cpp"
#include "../5/svm.cpp"
#include "tensor.hpp"
#include <unordered_map>


struct BP_MLP
{
	Tensor< double > w1, b1, w2, b2, x, y;
	TensorHolder< double > w1h, b1h, w2h, b2h, xh, yh, t5, t6, pre_y;
	TensorHolder< double > loss;
	const uint size1, size2, claz_size;
	double learn_rate = 0.01;
	BP_MLP( uint size1, uint size2, uint claz_size = 2, double learn_rate = 0.01 )
	    : size1( size1 ), size2( size2 ), claz_size( claz_size ), learn_rate( learn_rate ),
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
	double train( const Tensor< double > &x, const Tensor< double > &y )
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
		return loss.tensor.oneValue();
	}
	Tensor< double > operator()( const Tensor< double > &x )
	{
		this->x = x.copy();
		pre_y.force_cal();
		return pre_y.tensor;
	}
};

template < typename Data, typename Label, typename... Models >
struct Integrator
{
	Models models...;
	Integrator( Models... models ) : models( models... ) {}
	Integrator( const Integrator & ) = delete;
	Integrator &operator=( const Integrator & ) = delete;
	~Integrator() {}
	Label operator()( const Data &data )
	{
		std::unordered_map< Label, uint > count;
		( count[models( data )]++, ... );
		Label max_label;
		uint max_cnt = 0;
		for ( auto &i : count )
			if ( i.second > max_cnt )
				max_label = i.first, max_cnt = i.second;
		return max_label;
	}
};

int main()
{
	Integrator< Tensor< double >, uint >
}