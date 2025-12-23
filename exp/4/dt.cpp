#include "table/table.cc"
#include <cmath>
#include <map>
#include <string>
#include <unordered_map>
using std::map;
using std::string;
using std::unordered_map;
#define ID3_GRADE 5
#define COMMON_PROCESS                                    \
	if ( sub_data_size == 1 )                             \
	{                                                     \
		son = new DTN[1];                                 \
		son[0] = ( r_data[sub_data_idx[0]].Kind() );      \
		return 1;                                         \
	}                                                     \
	K most_kind;                                          \
	if ( !property_cnt )                                  \
	{                                                     \
		uint most_cnt = 0;                                \
		unordered_map< K, uint > class_size;              \
		for ( uint i = 0; i < sub_data_size; ++i )        \
			++class_size[r_data[sub_data_idx[i]].Kind()]; \
		for ( const auto &[key, cnt] : class_size )       \
			if ( cnt > most_cnt )                         \
				most_kind = key, most_cnt = cnt;          \
		son = new DTN[1];                                 \
		son[0] = std::move( *new DTN( most_kind ) );      \
		return 1;                                         \
	}

template < typename K, typename D >
struct Data
{
	K Kind() const;
	uint operator[]( uint i ) const;
	static double Entropy( const D *r_data, const uint *sub_data_idx, const uint &sub_data_size )
	{
		unordered_map< K, uint > kind_cnt;
		for ( uint i = 0; i < sub_data_size; ++i )
			++kind_cnt[r_data[sub_data_idx[i]].Kind()];
		double entropy = 0;
		for ( const auto &kv : kind_cnt )
			entropy -= (double)kv.second / sub_data_size * log2( (double)kv.second / sub_data_size );
		return entropy;
	}
	static double Entropy_Sum_of( const D *r_data, const uint *sub_data_idx, const uint &sub_data_size,
	                              const uint &property_class_cnt, const uint &property_index )
	{
		unordered_map< K, uint > *kind_cnt = new unordered_map< K, uint >[property_class_cnt];
		uint *property_class_size = new uint[property_class_cnt];
		uint i, j;
		for ( i = 0; i < property_class_cnt; ++i )
			property_class_size[i] = 0;
		for ( i = 0; i < sub_data_size; ++i )
		{
			j = r_data[sub_data_idx[i]][property_index];
			++kind_cnt[j][r_data[sub_data_idx[i]].Kind()];
			++property_class_size[j];
		}
		double entropy_sum = 0, entropy;
		for ( i = 0; i < property_class_cnt; ++i )
		{
			entropy = 0;
			for ( const auto &kv : kind_cnt[i] )
				entropy -= (double)kv.second / sub_data_size * log2( (double)kv.second / sub_data_size );
			entropy_sum += entropy * property_class_size[i] / sub_data_size;
		}
		delete[] property_class_size;
		delete[] kind_cnt;
		return entropy_sum;
	}

	static double Gain_and_Ratio( double &ratio, const double total_entropy, const D *r_data, const uint *sub_data_idx, const uint &sub_data_size,
	                              const uint &property_class_cnt, const uint &property_index )
	{
		// double gain = total_entropy - Entropy_Sum_of( r_data, sub_data_idx, sub_data_size, property_class_cnt, property_index );
		double gain = total_entropy, iv = 0;
		unordered_map< K, uint > *kind_cnt = new unordered_map< K, uint >[property_class_cnt];
		uint *property_class_size = new uint[property_class_cnt];
		uint i, j;
		for ( i = 0; i < property_class_cnt; ++i )
			property_class_size[i] = 0;
		for ( i = 0; i < sub_data_size; ++i )
		{
			j = r_data[sub_data_idx[i]][property_index];
			++kind_cnt[j][r_data[sub_data_idx[i]].Kind()];
			++property_class_size[j];
		}
		double entropy_sum = 0, entropy;
		for ( i = 0; i < property_class_cnt; ++i )
		{
			entropy = 0;
			for ( const auto &kv : kind_cnt[i] )
				entropy -= (double)kv.second / sub_data_size * log2( (double)kv.second / sub_data_size );
			gain -= entropy * property_class_size[i] / sub_data_size;
			if ( property_class_size[i] ) iv -= property_class_size[i] / sub_data_size * log2( property_class_size[i] / sub_data_size );
		}
		delete[] property_class_size;
		delete[] kind_cnt;
		if ( std::abs( iv ) <= 1e-100 )
			ratio = 1e100;
		else
			ratio = gain / iv;
		return gain;
	}

	template < typename S >
	static double Bi_Gain_and_Ratio( double &ratio, const S &split, const double total_entropy, const D *r_data, const uint *sub_data_idx, const uint &sub_data_size,
	                                 const uint &property_index )
	{
		// double gain = total_entropy - Entropy_Sum_of( r_data, sub_data_idx, sub_data_size, property_class_cnt, property_index );
		double gain = total_entropy, iv = 0;
		constexpr uint property_class_cnt = 2;
		unordered_map< K, uint > *kind_cnt = new unordered_map< K, uint >[property_class_cnt];
		uint *property_class_size = new uint[property_class_cnt];
		uint i, j;
		for ( i = 0; i < property_class_cnt; ++i )
			property_class_size[i] = 0;
		for ( i = 0; i < sub_data_size; ++i )
		{
			j = r_data[sub_data_idx[i]][property_index] >= split ? 1 : 0;
			++kind_cnt[j][r_data[sub_data_idx[i]].Kind()];
			++property_class_size[j];
		}
		double entropy_sum = 0, entropy;
		for ( i = 0; i < property_class_cnt; ++i )
		{
			entropy = 0;
			for ( const auto &kv : kind_cnt[i] )
				entropy -= (double)kv.second / sub_data_size * log2( (double)kv.second / sub_data_size );
			gain -= entropy * property_class_size[i] / sub_data_size;
			if ( property_class_size[i] ) iv -= property_class_size[i] / sub_data_size * log2( property_class_size[i] / sub_data_size );
		}
		delete[] property_class_size;
		delete[] kind_cnt;
		if ( std::abs( iv ) <= 1e-100 )
			ratio = 1e100;
		else
			ratio = gain / iv;
		return gain;
	}

	template < typename S >
	static double Bi_Gini( const S &split, const D *r_data, const uint *sub_data_idx, const uint &sub_data_size,
	                       const uint &property_index )
	{
		constexpr uint property_class_cnt = 2;
		unordered_map< K, uint > *kind_cnt = new unordered_map< K, uint >[property_class_cnt];
		uint *property_class_size = new uint[property_class_cnt];
		uint i, j;
		for ( i = 0; i < property_class_cnt; ++i )
			property_class_size[i] = 0;
		for ( i = 0; i < sub_data_size; ++i )
		{
			j = r_data[sub_data_idx[i]][property_index] >= split ? 1 : 0;
			++kind_cnt[j][r_data[sub_data_idx[i]].Kind()];
			++property_class_size[j];
		}
		double bgini = 0, gini;
		for ( i = 0; i < property_class_cnt; ++i )
		{
			gini = 1;
			for ( const auto &kv : kind_cnt[i] )
				gini -= (double)kv.second / sub_data_size * ( (double)kv.second / sub_data_size );
			bgini += gini * property_class_size[i] / sub_data_size;
		}
		delete[] property_class_size;
		delete[] kind_cnt;
		return gini;
	}
};


struct Cars : public Data< uint, Cars >
{
	uint property[8];

	Cars() = default;
	Cars( uint mpg, uint cylinders, uint cubicinches, uint hp, uint weightlbs, uint time_to_60, uint year, uint brand )
	    : property{ mpg,
	                cylinders,
	                cubicinches,
	                hp,
	                weightlbs,
	                time_to_60,
	                year,
	                brand } {}
	uint Kind() const { return property[7]; }
	uint operator[]( uint i ) const { return property[i]; }
};


typedef Table<
    double,
    int,
    double,
    double,
    double,
    double,
    int,
    string >
    Raw_Car_Table;
typedef Table<
    uint,
    uint,
    uint,
    uint,
    uint,
    uint,
    uint,
    uint >
    Car_Table;

//
template < typename T >
void class_col( Raw_Car_Table *raw, Car_Table *filled, const uint col_index[], uint *class_cnt, const uint size )
{
	TableColumnOperatorHelper< T, Raw_Car_Table > raw_column( *raw );
	TableColumnOperatorHelper< uint, Car_Table > filled_column( *filled );
	// todo :可以保存下映射以供分开处理训练和测试数据用
	if constexpr ( NotV< string, T > )
		for ( uint i = 0; i < size; ++i )
		{
			auto &raw_col = raw_column[col_index[i]];
			auto &filled_col = filled_column[col_index[i]];
			uint j = 0;
			map< T, ull > countMap;
			unordered_map< T, uint > classMap;
			for ( ull i = 0; i < raw_col.size; ++i )
				++countMap[raw_col[i]];
			for ( const auto &[key, cnt] : countMap )
				classMap[key] = j++;
			for ( ull j = 0; j < raw_column[i].size; ++j )
				filled_col.pushBack( classMap[raw_col[j]] );
			if ( class_cnt )
				class_cnt[i] = j;
		}
	else
		for ( uint i = 0; i < size; ++i )
		{
			auto &raw_col = raw_column[col_index[i]];
			auto &filled_col = filled_column[col_index[i]];
			uint j = 0;
			unordered_map< T, ull > countMap;
			unordered_map< T, uint > classMap;
			for ( ull i = 0; i < raw_col.size; ++i )
				++countMap[raw_col[i]];
			for ( const auto &[key, cnt] : countMap )
				classMap[key] = j++;
			for ( ull j = 0; j < raw_column[i].size; ++j )
				filled_col.pushBack( classMap[raw_col[j]] );
			if ( class_cnt )
				class_cnt[i] = j;
		}
}

template < typename T >
void grade_col( Raw_Car_Table *raw, Car_Table *filled, const uint col_index[], const uint size, const uint grade = 3 )
{
	if ( grade <= 1 ) return;
	TableColumnOperatorHelper< T, Raw_Car_Table > raw_column( *raw );
	TableColumnOperatorHelper< uint, Car_Table > filled_column( *filled );

	T gradeStep, gradeMin;
	bool isGradeEven = ( grade % 2 == 0 );
	for ( uint i = 0; i < size; ++i )
	{
		auto &raw_col = raw_column[col_index[i]];
		auto &filled_col = filled_column[col_index[i]];
		gradeMin = raw_col.min;
		gradeStep = ( raw_col.max - gradeMin ) / grade;
		for ( ull j = 0; j < raw_column[i].size; ++j )
			filled_col.pushBack( uint( ( raw_col[j] - gradeMin - 1e-6 ) / gradeStep ) );
	}
}
// template < typename D >
// using FSF = uint ( * )( const D &data );
template < typename D >
struct FSF
{
	virtual uint operator()( const D &data ) const = 0;
};
template < typename D >
struct FSF_by_Property_Class_as_Idx : public FSF< D >
{
	const uint property_index;
	FSF_by_Property_Class_as_Idx( const uint &property_index ) : property_index( property_index ) {}
	uint operator()( const D &data ) const { return data[this->property_index]; }
};
template < typename D, typename S >
struct FSF_by_Spliter : public FSF< D >
{
	const uint property_index;
	const S spliter;
	FSF_by_Spliter( const uint &property_index, const S &s ) : property_index( property_index ), spliter( s ) {}
	uint operator()( const D &data ) const { return data[this->property_index] >= this->spliter; }
};
template < typename D >
struct FSF_Directly : public FSF< D >
{
	const uint dr;
	FSF_Directly( const uint &s ) : dr( s ) {}
	uint operator()( const D &data ) const { return this->dr; }
};
// kind, data, arrange son func, find son func
template < typename K, typename D, template < typename, typename > typename ASF >
struct Decide_Tree_Node
{
	using DTN = Decide_Tree_Node< K, D, ASF >;

	uint son_size = 0;
	DTN *son = nullptr;
	FSF< D > *fsf = nullptr;
	K kind;

	Decide_Tree_Node( ASF< K, D > &&asf, const D *data,
	                  const uint *sub_data_idx, const uint &sub_data_size,
	                  const uint *proprty_class_size, const uint *property_index,
	                  const uint &property_cnt )
	{
		if ( !sub_data_size )
			throw std::runtime_error( "DTN: ?" );
		else if ( sub_data_size == 1 )
			this->kind = data[sub_data_idx[0]].Kind();
		else
			this->son_size = asf( data, sub_data_idx, sub_data_size,
			                      proprty_class_size, property_index, property_cnt,
			                      this->son, this->fsf );
	}
	Decide_Tree_Node( K kind )
	    : kind( kind ) {}
	Decide_Tree_Node() = default;
	DTN &operator=( const DTN &other ) = delete;
	DTN &operator=( DTN &&other )
	{
		this->kind = other.kind;
		this->son_size = other.son_size;
		this->fsf = other.fsf;
		this->son = other.son;
		other.son = nullptr;
		other.fsf = nullptr;
		return *this;
	}
	K Kind() const
	{
		if ( this->son_size )
			throw std::runtime_error( "DTN: ???" );
		else
			return this->son->Kind();
	}
	K operator()( const D &data ) const
	{
		if ( this->son_size > 1 )
			return this->son[this->fsf->operator()( data )].operator()( data );
		else if ( this->son_size == 1 )
			return this->son[0].operator()( data );
		else
			return this->kind;
	}
	~Decide_Tree_Node()
	{
		delete[] this->son;
		delete this->fsf;
	}
};
// template < typename Table, typename K >
// struct Decide_Tree
// {
// 	using LeafNode = Decide_Tree_Node< K >;
// 	Decide_Tree( const Table *r_data, const ull *sub_data_idx, const ull &sub_data_size ) = 0;
// 	K operator()( const Table &data ) const = 0;
// };
template < typename K, typename D >
struct ID3
{
	using DTN = Decide_Tree_Node< K, D, ID3 >;
	// uint operator()(
	//     const D *r_data, const uint *sub_data_idx, const uint &sub_data_size,
	//     const uint *proprty_class_size,
	//     const uint *property_index,
	//     const uint &property_cnt, DTN *&son, FSF< D > *&fsf = nullptr );
	// };
	ID3() = default;
	// template < typename K, typename D >
	uint operator()( const D *r_data, const uint *sub_data_idx, const uint &sub_data_size,
	                 const uint *property_class_size,
	                 const uint *property_index,
	                 const uint &property_cnt, DTN *&son, FSF< D > *&fsf )
	{
		COMMON_PROCESS
		uint best_property_index = 0, best_property_index_index = 0;
		double best_gain = 1e9, gain;
		for ( uint i = 0; i < property_cnt; ++i )
		{
			gain = D::Entropy_Sum_of( r_data, sub_data_idx, sub_data_size, property_class_size[property_index[i]], property_index[i] );
			if ( gain < best_gain )
				best_property_index = property_index[i], best_gain = gain, best_property_index_index = i;
		}

		uint *rest_property_index = new uint[property_cnt - 1];
		for ( uint i = 0, j = 0; i < property_cnt; ++i )
			if ( i != best_property_index_index )
				rest_property_index[j++] = property_index[i];

		son = new DTN[property_class_size[best_property_index]];
		fsf = new FSF_by_Property_Class_as_Idx< D >( best_property_index );

		uint sub_sub_data_size;
		uint *sub_sub_data_idx = new uint[sub_data_size];
		bool mostunfind = true;
		for ( uint i = 0; i < property_class_size[best_property_index]; ++i )
		{
			sub_sub_data_size = 0;
			for ( uint j = 0; j < sub_data_size; ++j )
				if ( r_data[sub_data_idx[j]][best_property_index] == i )
					sub_sub_data_idx[sub_sub_data_size++] = sub_data_idx[j];
			if ( !sub_sub_data_size )
			{
				if ( mostunfind )
				{
					uint most_cnt = 0;
					unordered_map< K, uint > class_size;
					for ( uint i = 0; i < sub_data_size; ++i )
						++class_size[r_data[sub_data_idx[i]].Kind()];
					for ( const auto &[key, cnt] : class_size )
						if ( cnt > most_cnt )
							most_kind = key, most_cnt = cnt;
				}
				son[i] = std::move( *new DTN( most_kind ) );
			}
			else
				son[i] = std::move( *new DTN( ID3(), r_data,
				                              sub_sub_data_idx, sub_sub_data_size,
				                              property_class_size, rest_property_index,
				                              property_cnt - 1 ) );
		}
		delete[] sub_sub_data_idx;
		delete[] rest_property_index;
		return property_class_size[best_property_index];
	}
};
template < typename K, typename D >
struct C4_5
{
	using DTN = Decide_Tree_Node< K, D, C4_5 >;
	// uint operator()(
	//     const D *r_data, const uint *sub_data_idx, const uint &sub_data_size,
	//     const uint *proprty_class_size,
	//     const uint *property_index,
	//     const uint &property_cnt, DTN *&son, FSF< D > *&fsf = nullptr );
	// };
	C4_5() = default;
	// template < typename K, typename D >
	uint operator()( const D *r_data, const uint *sub_data_idx, const uint &sub_data_size,
	                 const uint *property_class_size,
	                 const uint *property_index,
	                 const uint &property_cnt, DTN *&son, FSF< D > *&fsf )
	{
		COMMON_PROCESS
		uint best_property_index = 0, best_property_index_index = 0;
		double avg_gain = 0, *gain = new double[property_cnt], *gain_ratio = new double[property_cnt];
		uint *best_split_point = new uint[property_cnt];
		double total_entropy = D::Entropy( r_data, sub_data_idx, sub_data_size );
		for ( uint i = 0; i < property_cnt; ++i )
		{  // 连续
			if ( property_class_size[property_index[i]] > 10 )
			{
				double best_gain_ratio, best_gain = -1e9;
				double tmp_gain_ratio, tmp_gain = -1e9;
				uint bsp;
				for ( uint j = 0; j < property_class_size[property_index[i]]; ++j )
				{
					tmp_gain = D::Bi_Gain_and_Ratio( tmp_gain_ratio, j, total_entropy,
					                                 r_data, sub_data_idx, sub_data_size, property_index[i] );
					if ( tmp_gain > best_gain )
						best_gain = tmp_gain, best_gain_ratio = tmp_gain_ratio, bsp = j;
				}
				gain[i] = best_gain, gain_ratio[i] = best_gain_ratio, best_split_point[i] = bsp;
				avg_gain += best_gain;
			}
			else
			{
				gain[i] = D::Gain_and_Ratio( gain_ratio[i], total_entropy,
				                             r_data, sub_data_idx, sub_data_size, property_class_size[property_index[i]], property_index[i] );
				avg_gain += gain[i];
			}
		}
		double best_gain_ratio = -1e9;
		bool is_continuous = false;
		avg_gain /= property_cnt;
		for ( uint i = 0; i < property_cnt; ++i )
			if ( gain[i] >= avg_gain )
				if ( gain_ratio[i] > best_gain_ratio )
				{
					best_gain_ratio = gain_ratio[i];
					best_property_index = property_index[i];
					best_property_index_index = i;
					is_continuous = property_class_size[best_property_index] > 10;
				}

		uint *rest_property_index = new uint[property_cnt - 1];
		for ( uint i = 0, j = 0; i < property_cnt; ++i )
			if ( i != best_property_index_index )
				rest_property_index[j++] = property_index[i];


		if ( is_continuous )
		{
			son = new DTN[2];

			uint sub_sub_data_size;
			uint *sub_sub_data_idx = new uint[sub_data_size];
			uint has_empty = 0;

			sub_sub_data_size = 0;
			for ( uint j = 0; j < sub_data_size; ++j )
				if ( r_data[sub_data_idx[j]][best_property_index] < best_split_point[best_property_index_index] )
					sub_sub_data_idx[sub_sub_data_size++] = sub_data_idx[j];
			if ( !sub_sub_data_size )
				has_empty = 2;
			else
				son[0] = std::move( *new DTN( C4_5(), r_data,
				                              sub_sub_data_idx, sub_sub_data_size,
				                              property_class_size, rest_property_index,
				                              property_cnt - 1 ) );
			sub_sub_data_size = 0;
			for ( uint j = 0; j < sub_data_size; ++j )
				if ( r_data[sub_data_idx[j]][best_property_index] >= best_split_point[best_property_index_index] )
					sub_sub_data_idx[sub_sub_data_size++] = sub_data_idx[j];
			if ( !sub_sub_data_size )
				has_empty = 1;
			else
				son[1] = std::move( *new DTN( C4_5(), r_data,
				                              sub_sub_data_idx, sub_sub_data_size,
				                              property_class_size, rest_property_index,
				                              property_cnt - 1 ) );


			delete[] sub_sub_data_idx;
			delete[] rest_property_index;
			if ( has_empty )
				fsf = new FSF_Directly< D >( has_empty - 1 );
			else
				fsf = new FSF_by_Spliter< D, uint >( best_property_index, best_split_point[best_property_index_index] );
			return 2;
		}
		else
		{
			son = new DTN[property_class_size[best_property_index]];
			fsf = new FSF_by_Property_Class_as_Idx< D >( best_property_index );
			uint sub_sub_data_size;
			uint *sub_sub_data_idx = new uint[sub_data_size];
			bool mostunfind = true;
			for ( uint i = 0; i < property_class_size[best_property_index]; ++i )
			{
				sub_sub_data_size = 0;
				for ( uint j = 0; j < sub_data_size; ++j )
					if ( r_data[sub_data_idx[j]][best_property_index] == i )
						sub_sub_data_idx[sub_sub_data_size++] = sub_data_idx[j];
				if ( !sub_sub_data_size )
				{
					if ( mostunfind )
					{
						uint most_cnt = 0;
						unordered_map< K, uint > class_size;
						for ( uint i = 0; i < sub_data_size; ++i )
							++class_size[r_data[sub_data_idx[i]].Kind()];
						for ( const auto &[key, cnt] : class_size )
							if ( cnt > most_cnt )
								most_kind = key, most_cnt = cnt;
					}
					son[i] = std::move( *new DTN( most_kind ) );
				}
				else
					son[i] = std::move( *new DTN( C4_5(), r_data,
					                              sub_sub_data_idx, sub_sub_data_size,
					                              property_class_size, rest_property_index,
					                              property_cnt - 1 ) );
			}

			delete[] sub_sub_data_idx;
			delete[] rest_property_index;
			return property_class_size[best_property_index];
		}
	}
};
// 都二分
template < typename K, typename D >
struct CART
{
	using DTN = Decide_Tree_Node< K, D, CART >;
	// uint operator()(
	//     const D *r_data, const uint *sub_data_idx, const uint &sub_data_size,
	//     const uint *proprty_class_size,
	//     const uint *property_index,
	//     const uint &property_cnt, DTN *&son, FSF< D > *&fsf = nullptr );
	// };
	CART() = default;
	// template < typename K, typename D >
	uint operator()( const D *r_data, const uint *sub_data_idx, const uint &sub_data_size,
	                 const uint *property_class_size,
	                 const uint *property_index,
	                 const uint &property_cnt, DTN *&son, FSF< D > *&fsf )
	{
		COMMON_PROCESS
		uint best_property_index = 0, best_property_index_index = 0;
		double gini = 1e9;
		uint best_split_point;
		for ( uint i = 0; i < property_cnt; ++i )
		{
			double best_gini = 1e9, tmp_gini;
			uint bsp;
			for ( uint j = 0; j < property_class_size[property_index[i]]; ++j )
			{
				tmp_gini = D::Bi_Gini( j,
				                       r_data, sub_data_idx, sub_data_size, property_index[i] );
				if ( tmp_gini < best_gini )
					best_gini = tmp_gini, bsp = j;
			}
			if ( best_gini < gini ) gini = best_gini, best_split_point = bsp, best_property_index = property_index[i], best_property_index_index = i;
		}
		uint *rest_property_index = new uint[property_cnt - 1];
		for ( uint i = 0, j = 0; i < property_cnt; ++i )
			if ( i != best_property_index_index )
				rest_property_index[j++] = property_index[i];

		son = new DTN[2];

		uint sub_sub_data_size;
		uint *sub_sub_data_idx = new uint[sub_data_size];
		uint has_empty = 0;

		sub_sub_data_size = 0;
		for ( uint j = 0; j < sub_data_size; ++j )
			if ( r_data[sub_data_idx[j]][best_property_index] < best_split_point )
				sub_sub_data_idx[sub_sub_data_size++] = sub_data_idx[j];
		if ( !sub_sub_data_size )
			has_empty = 2;
		else
			son[0] = std::move( *new DTN( CART(), r_data,
			                              sub_sub_data_idx, sub_sub_data_size,
			                              property_class_size, rest_property_index,
			                              property_cnt - 1 ) );
		sub_sub_data_size = 0;
		for ( uint j = 0; j < sub_data_size; ++j )
			if ( r_data[sub_data_idx[j]][best_property_index] >= best_split_point )
				sub_sub_data_idx[sub_sub_data_size++] = sub_data_idx[j];
		if ( !sub_sub_data_size )
			has_empty = 1;
		else
			son[1] = std::move( *new DTN( CART(), r_data,
			                              sub_sub_data_idx, sub_sub_data_size,
			                              property_class_size, rest_property_index,
			                              property_cnt - 1 ) );


		delete[] sub_sub_data_idx;
		delete[] rest_property_index;
		if ( has_empty )
			fsf = new FSF_Directly< D >( has_empty - 1 );
		else
			fsf = new FSF_by_Spliter< D, uint >( best_property_index, best_split_point );
		return 2;
	}
};
#ifndef INTEGRATE

int main()
{
	Raw_Car_Table *raw_car_table = Raw_Car_Table::FromCSV( "./cars.csv" );
	uint i = 0;
	uint train_size = raw_car_table->dataRow * 4 / 5;
	uint test_size = raw_car_table->dataRow - train_size;
	Cars *train_data = new Cars[train_size];
	uint *train_index = new uint[train_size];
	Cars *test_data = new Cars[test_size];
	uint *test_index = new uint[test_size];
	Car_Table *car_table = new Car_Table( true, raw_car_table->dataRow );
	car_table->header = new unordered_map< string, uint >( *raw_car_table->header );
	uint class_cnt[7], raw_int_class_cnt[2], raw_double_class_cnt[5];
	class_col< int >( raw_car_table, car_table, (uint[]){ 1, 6 }, raw_int_class_cnt, 2 );
	class_col< double >( raw_car_table, car_table, (uint[]){ 0, 2, 3, 4, 5 }, raw_double_class_cnt, 5 );
	class_col< string >( raw_car_table, car_table, (uint[]){ 7 }, nullptr, 1 );
	class_cnt[0] = raw_double_class_cnt[0];
	class_cnt[1] = raw_int_class_cnt[0];
	class_cnt[2] = raw_double_class_cnt[1];
	class_cnt[3] = raw_double_class_cnt[2];
	class_cnt[4] = raw_double_class_cnt[3];
	class_cnt[5] = raw_double_class_cnt[4];
	class_cnt[6] = raw_int_class_cnt[1];
	car_table->toCSV( "./cars_classed.csv" );
	cout << "property class count :\n";
	for ( uint i = 0; i < 7; ++i )
		cout << class_cnt[i] << endl;
#ifdef ID3_GRADE
	Car_Table *car_table_graded = new Car_Table( true, raw_car_table->dataRow );
	car_table_graded->header = new unordered_map< string, uint >( *raw_car_table->header );

	uint class_graded_cnt[7];
	class_col< int >( raw_car_table, car_table_graded, (uint[]){ 1, 6 }, raw_int_class_cnt, 2 );
	grade_col< double >( raw_car_table, car_table_graded, (uint[]){ 0, 2, 3, 4, 5 }, 5,
	                     ID3_GRADE );
	class_col< string >( raw_car_table, car_table_graded, (uint[]){ 7 }, nullptr, 1 );
	for ( uint i = 0; i < 7; ++i )
		class_graded_cnt[i] = ID3_GRADE;
	class_graded_cnt[1] = raw_int_class_cnt[0];
	class_graded_cnt[6] = raw_int_class_cnt[1];
	car_table_graded->toCSV( "./cars_graded.csv" );
#endif
	// todo random access
	for ( ; i < train_size; ++i )
		train_data[i] = car_table->template row< Cars >( i ), train_index[i] = i;
	for ( uint j = 0; i < raw_car_table->dataRow; ++i, ++j )
		test_data[j] = car_table->template row< Cars >( i ), test_index[j] = j;

#ifndef ID3_GRADE
	auto id3_p = new Decide_Tree_Node< uint, Cars, ID3< uint, Cars > >( ID3< uint, Cars >(), train_data, train_index, train_size,
	                                                                    class_cnt, (uint[]){ 0, 1, 2, 3, 4, 5, 6 }, 7U );
	auto &id3 = *id3_p;
	uint right = 0;
	for ( uint i = 0; i < test_size; ++i )
		if ( id3( test_data[i] ) == test_data[i].Kind() )
			++right;
	cout << "ID3: " << (double)right / test_size << endl;
#else
	Cars *train_data_graded = new Cars[train_size];
	Cars *test_data_graded = new Cars[test_size];
	for ( i = 0; i < train_size; ++i )
		train_data_graded[i] = car_table_graded->template row< Cars >( i );
	for ( uint j = 0; i < raw_car_table->dataRow; ++i, ++j )
		test_data_graded[j] = car_table_graded->template row< Cars >( i );
	auto id3_p = new Decide_Tree_Node< uint, Cars, ID3 >( ID3< uint, Cars >(), train_data_graded, train_index, train_size,
	                                                                    class_cnt, (uint[]){ 0, 1, 2, 3, 4, 5, 6 }, 7U );
	auto &id3 = *id3_p;
	uint right = 0;
	for ( uint i = 0; i < test_size; ++i )
		if ( id3( test_data_graded[i] ) == test_data[i].Kind() )
			++right;
	cout << "ID3: " << (double)right / test_size << endl;
	delete car_table_graded;
	delete[] train_data_graded;
	delete[] test_data_graded;
#endif

	auto c4_5_p = new Decide_Tree_Node< uint, Cars, C4_5 >( C4_5< uint, Cars >(), train_data, train_index, train_size,
	                                                                      class_cnt, (uint[]){ 0, 1, 2, 3, 4, 5, 6 }, 7U );
	auto &c4_5 = *c4_5_p;
	right = 0;
	for ( uint i = 0; i < test_size; ++i )
		if ( c4_5( test_data[i] ) == test_data[i].Kind() )
			++right;
	cout << "C4.5: " << (double)right / test_size << endl;
	auto cart_p = new Decide_Tree_Node< uint, Cars, CART >( CART< uint, Cars >(), train_data, train_index, train_size,
	                                                                      class_cnt, (uint[]){ 0, 1, 2, 3, 4, 5, 6 }, 7U );
	auto &cart = *cart_p;
	right = 0;
	for ( uint i = 0; i < test_size; ++i )
		if ( cart( test_data[i] ) == test_data[i].Kind() )
			++right;
	cout << "CART: " << (double)right / test_size << endl;
	delete c4_5_p;
	delete id3_p;
	delete car_table;
	delete[] train_data;
	delete[] train_index;
	delete[] test_data;
	delete[] test_index;
	delete raw_car_table;
}
#endif