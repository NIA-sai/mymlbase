#include "../N_dis_utils.cpp"
#include "arr.cc"
#include "tensor/tensor.cc"
#include <cmath>
#include <queue>
#include <utility>
using std::cout;
using std::endl;
using dis_caler2_func_p = double ( * )( uint, const Tensor< double > &, const Tensor< double > & );
auto cmp = []( const std::pair< double, uint > &a, const std::pair< double, uint > &b ) -> bool
{
	return a.first < b.first;
};
using pq = std::priority_queue< std::pair< double, uint >, std::vector< std::pair< double, uint > >, decltype( cmp ) >;
double eu_dis2( uint dimSize, const Tensor< double > &a, const Tensor< double > &b )
{
	double sum = 0;
	for ( uint i = 0; i < dimSize; ++i )
		sum += pow( a.data( i ) - b.data( i ), 2 );
	return sum;
}

auto maha_dis2_func( const Tensor< double > &cov )
{
	// 线性相关剔除，求伪逆暂未实现
	return [&cov]( uint dimSize, const Tensor< double > &a, const Tensor< double > &b ) -> double
	{
		// Tensor< double > d = a - b;
		// Tensor< double > tmp = Tensor< double >::solve( cov, d );
		// return ( d.transpose() * tmp ).oneValue();
	};
}
template < class Dis_Caler2_Func = dis_caler2_func_p >
uint knn_classify( uint k, uint dimSize, const Tensor< double > test, const Tensor< double > **clazs_sample, uint *claz_sample_cnt, uint clazs_cnt, Dis_Caler2_Func dis_caler2 = eu_dis2 )
{
	pq kn( cmp );
	double dis;
	int *clazs_cnter = new int[clazs_cnt];
	for ( uint i = 0; i < clazs_cnt; ++i )
	{
		clazs_cnter[i] = 0;
		for ( uint j = 0; j < claz_sample_cnt[i]; ++j )
		{
			dis = dis_caler2( dimSize, test, clazs_sample[i][j] );
			if ( kn.size() < k )
			{
				++clazs_cnter[i];
				kn.push( { dis, i } );
			}
			else if ( dis < kn.top().first )
			{
				--clazs_cnter[kn.top().second];
				++clazs_cnter[i];
				kn.pop();
				kn.push( { dis, i } );
			}
		}
	}
	uint max = 0, max_claz = 0;
	for ( uint i = 0; i < clazs_cnt; ++i )
		if ( clazs_cnter[i] > max )
		{
			max = clazs_cnter[i];
			max_claz = i;
		}
	delete[] clazs_cnter;
	return max_claz;
}
int main()
{
	Arr< std::string > *raw_data_arr = Arr< std::string >::FromCSV( "./diagnosis_result.csv" );
	Tensor< std::string > *raw_data = new Tensor< std::string >( { 101, 10 }, raw_data_arr->data );
	Tensor< double > *data_p = new Tensor( raw_data->operator()( 1, 101 )( 2, 9, 1, 2 ).to< double >() );
	Tensor< double > &data = *data_p;
	// 标准正态分布归一化data
	N_dis_normalizeT2( data );
	// 分类data_sample
	Tensor< double > **data_sample = new Tensor< double > *[2];
	data_sample[0] = new Tensor< double >[100];
	data_sample[1] = new Tensor< double >[100];
	uint *claz_sample_cnt = new uint[2];
	claz_sample_cnt[0] = 0;
	claz_sample_cnt[1] = 0;
	for ( uint i = 1; i < 101; ++i )
	{
		std::string s = raw_data->data( i, 1 );
		if ( s == "Positive" )
			data_sample[1][claz_sample_cnt[1]++] = data[i - 1];
		else
			data_sample[0][claz_sample_cnt[0]++] = data[i - 1];
	}
	for ( uint i = 0; i < 2; ++i )
		for ( uint j = 0; j < claz_sample_cnt[i]; ++j )
			cout << data_sample[i][j] << endl;


	delete[] data_sample[0];
	delete[] data_sample[1];
	delete[] data_sample;
	delete data_p;
	delete raw_data;
	delete raw_data_arr;

	
}