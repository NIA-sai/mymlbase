#include "tensor/tensor.cc"
// #include <matioCpp/matioCpp.h>
using std::abs;
using std::max;
using std::min;
#define ZERO 1e-10
// struct comparer
// {
// 	bool operator()( const std::pair< uint, double > &a, const std::pair< uint, double > &b )
// 	{
// 		return a.second < b.second;
// 	}
// };

// using pq = std::priority_queue< std::pair< uint, double >, std::vector< std::pair< uint, double > >, comparer >;

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
struct Kernel
{
	void operator()( const Tensor< double > &, const Tensor< double > & ) const;
};
template < typename K >
struct Solver_Func
{
	void operator()( K &, const double &, const Tensor< double > &, const Tensor< double > &, Tensor< double > &, Tensor< double > &, Tensor< double > &, Tensor< double > & );
};

struct Liner_Kernel : public Kernel
{
	Tensor< double > operator()( const Tensor< double > &X, const Tensor< double > &sv ) const
	{
		return X * sv.transpose();
	}
};


struct Gaussian_Kernel : public Kernel
{
	double gamma = 1.0;
	Gaussian_Kernel() = default;
	Gaussian_Kernel( double gamma )
	    : gamma( gamma ) {}

	Tensor< double > operator()( const Tensor< double > &X,
	                             const Tensor< double > &sv ) const
	{
		Tensor< double > dot = X * sv.transpose() * 2.0;  // (n, m)

		Tensor< double > X_norm = X.eMul( X ).sum( 1 );  // (n, 1)
		// cout << sv.eMul( sv ).sum( 1 );
		// cout << sv.eMul( sv ).sum( 1 ).transpose();
		Tensor< double > sv_norm = sv.eMul( sv ).sum( 1 ).transpose().copy();  // (1, m)

		Tensor< double > dist2 = X_norm + sv_norm - dot;

		return Tensor< double >::Exp( dist2 * ( -gamma ) );  // (n, m)
	}
};

template < typename K >
struct SMO : public Solver_Func< K >
{
	void operator()( K &k, const double &C, const Tensor< double > &X, const Tensor< double > &y, Tensor< double > &a, Tensor< double > &b, Tensor< double > &sv, Tensor< double > &svy )
	{
		const uint size = y.shape.dimsSize[0];
		uint i = 0, a_1, a_2;
		double max_vio, vio, l, r, ita, k_11, k_22, k_12, da_2, m_da_1, m_db;
		a = Tensor< double >::All_of( y.shape, 0.0 );
		b.r_data[0] = 0.0;

		Tensor< double > *E_p = new Tensor< double >();
		Tensor< double > &E = *E_p;
		E = -y;
		// pq = new pq();
		// for ( ; i < size; ++i )
		// pq.push( { i, 0.0 } );
		// todo: 暂无接口，不支持显卡加速
		// for ( ; i < size; ++i )
		//  k( X[i], X ).eMul( a.eMul( svy ) ).sum() + b;
		for ( uint ii = 0; ii < size; ++ii )
		{
			max_vio = 0;
			for ( i = 0; i < size; ++i )
			{
				if ( a.data( i ) <= ZERO )
					vio = max( -E.r_data[i], 0.0 );
				else if ( a.r_data[i] >= C - ZERO )
					vio = max( E.r_data[i], 0.0 );
				else
					vio = abs( E.r_data[i] );
				if ( vio >= max_vio )
					max_vio = vio, a_1 = i;
			}
			if ( max_vio < ZERO )
				break;
			// cout << E<<E[0];
			double max_dE = 0;
			for ( i = 0; i < size; ++i )
				if ( std::abs( E.r_data[i] - E.r_data[a_1] ) >= max_dE )
					max_dE = std::abs( E.r_data[i] - E.r_data[a_1] ), a_2 = i;
			// a_2 = ( E - E[a_1] ).abs().index_of_max( 1 ).oneValue();

			if ( y.data( a_1 ) == y.data( a_2 ) )
				l = max( -a.r_data[a_2], a.r_data[a_1] - C ), r = min( C - a.r_data[a_2], a.r_data[a_1] );
			else
				l = max( -a.r_data[a_2], -a.r_data[a_1] ), r = min( C - a.r_data[a_2], C - a.r_data[a_1] );
			if ( l + ZERO <= r )
			{
				//
				const Tensor< double > &x_a_1 = X( a_1 );
				const Tensor< double > &x_a_2 = X( a_2 );
				k_11 = k( x_a_1, x_a_1 ).oneValue();
				k_22 = k( x_a_2, x_a_2 ).oneValue();
				k_12 = k( x_a_1, x_a_2 ).oneValue();
				ita = k_11 + k_22 - 2.0 * k_12;
				if ( ita <= ZERO )
					ita = ZERO;
				// cout << E.r_data[a_1] - E.r_data[a_2];
				// cout << y.data( a_2 ) * ( E.r_data[a_1] - E.r_data[a_2] ) / ita;
				// cout << l;
				da_2 = min( r, max( l, y.data( a_2 ) * ( E.r_data[a_1] - E.r_data[a_2] ) / ita ) );
				a.r_data[a_2] += da_2;
				m_da_1 = y.data( a_1 ) * y.data( a_2 ) * da_2;
				a.r_data[a_1] -= m_da_1;

				if ( a.r_data[a_1] > ZERO && a.r_data[a_1] <= C - ZERO )
					m_db = E.r_data[a_1] - y.data( a_1 ) * m_da_1 * k_11 + y.data( a_2 ) * da_2 * k_12;
				else if ( a.r_data[a_2] > ZERO && a.r_data[a_2] <= C - ZERO )
					m_db = E.r_data[a_2] - y.data( a_1 ) * m_da_1 * k_12 + y.data( a_2 ) * da_2 * k_22;
				else
					m_db = ( E.r_data[a_1] + E.r_data[a_2] - y.data( a_1 ) * m_da_1 * ( k_11 + k_12 ) - y.data( a_2 ) * da_2 * ( k_22 + k_12 ) ) / 2.0;
				b.r_data[0] -= m_db;
				for ( i = 0; i < size; ++i )
				{
					const Tensor< double > &x_i = X( i );
					E.r_data[i] -= y.data( a_1 ) * m_da_1 * k( x_i, x_a_1 ).oneValue() - y.data( a_2 ) * da_2 * k( x_i, x_a_2 ).oneValue() + m_db;
				}
			}
		}
		uint sv_size = 0, *sv_index = new uint[size];
		for ( i = 0; i < size; ++i )
			if ( a.r_data[i] >= ZERO )
				sv_index[sv_size++] = i;
		sv = X( sv_index, sv_size );
		svy = y( sv_index, sv_size );
		a = a( sv_index, sv_size );
		delete E_p;
	}
};




template < template < typename > typename F, typename K = Liner_Kernel >
struct SVM
{
	F< K > f;
	K k;
	Tensor< double > *a_p, b;
	Tensor< double > *sv_p, *svy_p;
	Tensor< double > &a, &sv, &svy;
	const double C = 1.0;
	SVM( double C = 1.0, K &&k = K(), F< K > &&f = F< K >() ) : C( C ), f( std::forward< F< K > >( f ) ), k( std::forward< K >( k ) ), a_p( new Tensor< double >() ), b( Tensor< double >::OneValue( 0.0 ) ), sv_p( new Tensor< double >() ), svy_p( new Tensor< double >() ), a( *a_p ), sv( *sv_p ), svy( *svy_p ) {}
	void fit( const Tensor< double > &X, const Tensor< double > &y )
	{
		// 以y的数量为准
		if ( X.shape.dimsSize[0] > y.shape.dimsSize[0] )
			// throw std::runtime_error( "?" );
			cout << "?\n";
		f( k, C, X, y, a, b, sv, svy );
	}
	Tensor< double > operator()( const Tensor< double > &X )
	{
		// cout << a.eMul( svy );
		// cout << sv;
		// cout << k( X, sv ).size;
		return k( X, sv ).eMul( a.eMul( svy ).transpose() ).sum( 1 ) + b;
	}
	// test
	friend std::ostream &operator<<( std::ostream &os, const SVM &s )
	{
		// os << s.a;
		// os << s.b;
		// os << s.sv;
		// os << s.svy;
		// os << s.svy.eMul( s.a );
		// os << s.svy.eMul( s.sv );
		// os << s.a.eMul( s.sv ).eMul( s.svy ).sum( 0 );
		return os;
	}
	~SVM()
	{
		delete &a;
		delete &sv;
		delete &svy;
	}
};
#ifndef INTEGRATE
int main()
{
	Tensor< double > *train_X, *train_y;
	// load_data( "./ex5data1.mat", train_X, train_y );
	// train_X->toCSV( "train_X.csv" );
	// train_y->toCSV( "train_y.csv" );

	Tensor< double > *test_X, *test_y;
	// load_data( "./ex5data2.mat", test_X, test_y );
	// test_X->toCSV( "test_X.csv" );
	// test_y->toCSV( "test_y.csv" );

	train_X = new Tensor< double >();
	train_y = new Tensor< double >();
	test_X = new Tensor< double >();
	test_y = new Tensor< double >();
	*train_X = Tensor< double >::FromCSV( "train_X.csv", false );
	*train_y = Tensor< double >::FromCSV( "train_y_rl.csv", false );
	*test_X = Tensor< double >::FromCSV( "test_X.csv", false );
	*test_y = Tensor< double >::FromCSV( "test_y_rl.csv", false );

	train_y->reshape( { { (uint)train_y->size, 1U } } );
	test_y->reshape( { { (uint)test_y->size, 1U } } );
	// *test_y *= 2;
	// *train_y *= 2;
	// *test_y -= 1;
	// *train_y -= 1;
	// test_y->toCSV( "test_y_np.csv" );
	// train_y->toCSV( "train_y_np.csv" );
	// todo 归一化参数
	constexpr double C = 2;
	SVM< SMO > svm( C );
	svm.fit( *train_X, *train_y );
	auto pre_y = svm( *test_X );
	// pre_y.reshape( { { (uint)test_y->size } } );
	// cout << pre_y;
	cout << "accuracy: " << ( pre_y.eMul( *test_y ) > 0 ).sum().oneValue() / test_y->size << endl;
	cout << svm.sv << svm.svy;

	SVM< SMO, Gaussian_Kernel > gsvm( C, Gaussian_Kernel( 40.0 ) );
	gsvm.fit( *train_X, *train_y );
	auto gpre_y = gsvm( *test_X );
	// gpre_y.reshape( { { (uint)test_y->size } } );
	cout << "g accuracy: " << ( gpre_y.eMul( *test_y ) > 0 ).sum().oneValue() / test_y->size << endl;
	// cout << gsvm.sv;
	delete test_X;
	delete test_y;
	delete train_X;
	delete train_y;
	return 0;
}
#endif
/*
 g++ -I "d:\Code\Cpp\Projects\ml\mymlbase/include/" -I "d:\Code\Cpp\Projects\ml\mymlbase/vcpkg_installed/x64-mingw-dynamic/include/" -L "d:\Code\Cpp\Projects\ml\mymlbase/vcpkg_installed/x64-mingw-dynamic/lib/"  -std=c++20  svm.cpp -lmatioCpp  -o "../../output/svm"
../../output/svm.exe
 */