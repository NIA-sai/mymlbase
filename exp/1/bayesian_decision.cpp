#include "loan_record.cc"
#include "smooth_bayes_class_pos.cc"
#include "table/table.cc"
#include <string>


using std::string;



typedef Table<
    Optional< long long >,  // loan_id
    Optional< long long >,  // user_id
    Optional< double >,     // total_loan
    Optional< int >,        // year_of_loan
    Optional< double >,     // interest
    Optional< double >,     // monthly_payment
    Optional< char >,       // class
    Optional< string >,     // employer_type
    Optional< string >,     // industry
    Optional< string >,     // work_year
    Optional< bool >,       // house_exist
    Optional< bool >,       // censor_status
    Optional< string >,     // issue_date
    Optional< int >,        // use
    Optional< int >,        // post_code
    Optional< int >,        // region
    Optional< double >,     // debt_loan_ratio
    Optional< int >,        // del_in_18month
    Optional< double >,     // scoring_low
    Optional< double >,     // scoring_high
    Optional< int >,        // known_outstanding_loan
    Optional< int >,        // known_dero
    Optional< int >,        // pub_dero_bankrup
    Optional< double >,     // recircle_b
    Optional< double >,     // recircle_u
    Optional< bool >,       // initial_list_status
    Optional< bool >,       // app_type
    Optional< string >,     // earlies_credit_mon
    Optional< int >,        // title
    Optional< int >,        // policy_code
    Optional< int >,        // f0
    Optional< int >,        // f1
    Optional< int >,        // f2
    Optional< int >,        // f3
    Optional< int >,        // f4
    Optional< int >,        // early_return
    Optional< int >,        // early_return_amount
    Optional< double >,     // early_return_amount_3mon
    bool                    // isDefault
    >
    Raw_Loan_Record_Table;
typedef Table<
    Optional< long long >,  // loan_id
    Optional< long long >,  // user_id
    Optional< double >,     // total_loan
    Optional< int >,        // year_of_loan
    Optional< double >,     // interest
    Optional< double >,     // monthly_payment
    Optional< char >,       // class
    Optional< string >,     // employer_type
    Optional< string >,     // industry
    Optional< string >,     // work_year
    Optional< bool >,       // house_exist
    Optional< bool >,       // censor_status
    Optional< string >,     // issue_date
    Optional< int >,        // use
    Optional< int >,        // post_code
    Optional< int >,        // region
    Optional< double >,     // debt_loan_ratio
    Optional< int >,        // del_in_18month
    Optional< double >,     // scoring_low
    Optional< double >,     // scoring_high
    Optional< int >,        // known_outstanding_loan
    Optional< int >,        // known_dero
    Optional< int >,        // pub_dero_bankrup
    Optional< double >,     // recircle_b
    Optional< double >,     // recircle_u
    Optional< bool >,       // initial_list_status
    Optional< bool >,       // app_type
    Optional< string >,     // earlies_credit_mon
    Optional< int >,        // title
    Optional< int >,        // policy_code
    Optional< int >,        // f0
    Optional< int >,        // f1
    Optional< int >,        // f2
    Optional< int >,        // f3
    Optional< int >,        // f4
    Optional< int >,        // early_return
    Optional< int >,        // early_return_amount
    Optional< double >,     // early_return_amount_3mon
    bool,                   // isDefault
    bool                    // isDefault(guess)
    >
    Raw_Loan_Record_Table_And_Guess;
typedef Table<
    int,   // ll // loan_id
    int,   // ll  // user_id
    int,   //       // total_loan
    int,   // year_of_loan
    int,   //       // interest
    int,   //       // monthly_payment
    char,  // class
    int,   // str  // employer_type
    int,   // str  // industry
    int,   // str  // work_year
    bool,  // house_exist
    bool,  // censor_status
    int,   // issue_date_year
    int,   // use
    int,   // post_code
    int,   // region
    int,   //       // debt_loan_ratio
    int,   // del_in_18month
    int,   //       // scoring_low
    int,   //       // scoring_high
    int,   // known_outstanding_loan
    int,   // known_dero
    int,   // pub_dero_bankrup
    int,   //       // recircle_b
    int,   //       // recircle_u
    bool,  // initial_list_status
    bool,  // app_type
    int,   // str  // earlies_credit_mon
    int,   // title
    int,   // policy_code
    int,   // f0
    int,   // f1
    int,   // f2
    int,   // f3
    int,   // f4
    int,   // early_return
    int,   // early_return_amount
    int,   //       // early_return_amount_3mon
    bool,  // isDefault,
    int,   // issue_date_month
    int    // issue_date_day
    >
    Loan_Record_Table;
const uint ll_col_index[] = { 0, 1 };
const uint int_col_index[] = { 3, 13, 14, 15, 17, 20, 21, 22, 28, 29, 30, 31, 32, 33, 34, 35, 36 };
const uint double_col_index[] = { 2, 4, 5, 16, 18, 19, 23, 24, 37 };
const uint char_col_index[] = { 6 };
const uint string_col_index[] = { 7, 8, 9, 27 };
const uint bool_col_index[] = { 10, 11, 25, 26 };


// todo
template < typename T >
void avg_fill( Raw_Loan_Record_Table *raw, Loan_Record_Table *filled, const uint col_index[], const uint size )
{
	TableColumnOperatorHelper< Optional< T >, Raw_Loan_Record_Table > raw_column( *raw );
	TableColumnOperatorHelper< T, Loan_Record_Table > filled_column( *filled );
	for ( uint i = 0; i < size; ++i )
	{
		auto &raw_col = raw_column[col_index[i]];
		auto &filled_col = filled_column[col_index[i]];
		for ( ull j = 0; j < raw_column[i].size; ++j )
		{
			if ( !raw_col[j].hasValue() )
				filled_col.pushBack( raw_col.Avg() );
			else
				filled_col.pushBack( raw_col[j].value );
		}
	}
}

template < typename T >
T common_value_in_map( const std::unordered_map< T, ull > &countMap )
{
	T maxVal;
	ull maxCount = 0;
	for ( const auto &[key, cnt] : countMap )
	{
		if ( cnt > maxCount )
		{
			maxCount = cnt;
			maxVal = key;
		}
	}

	return maxVal;
}
template < typename T >
T common_value_in( const Column< Optional< T > > &col )
{
	if ( col.size == 0 ) return T();
	std::unordered_map< T, ull > countMap;
	for ( ull i = 0; i < col.size; ++i )
	{
		if ( col[i].hasValue() )
			++countMap[col[i].value];
	}

	T maxVal;
	ull maxCount = 0;
	for ( const auto &[key, cnt] : countMap )
	{
		if ( cnt > maxCount )
		{
			maxCount = cnt;
			maxVal = key;
		}
	}

	return maxVal;
}
template < typename T >
void common_fill( Raw_Loan_Record_Table *raw, Loan_Record_Table *filled, const uint col_index[], const uint size )
{
	TableColumnOperatorHelper< Optional< T >, Raw_Loan_Record_Table > raw_column( *raw );
	TableColumnOperatorHelper< T, Loan_Record_Table > filled_column( *filled );
	for ( uint i = 0; i < size; ++i )
	{
		auto &raw_col = raw_column[col_index[i]];
		auto &filled_col = filled_column[col_index[i]];
		T commonVal = common_value_in( raw_col );
		for ( ull j = 0; j < raw_column[i].size; ++j )
		{
			if ( !raw_col[j].hasValue() )
				filled_col.pushBack( commonVal );
			else
				filled_col.pushBack( raw_col[j].value );
		}
	}
}
template < typename T >
void common_fill_and_class( Raw_Loan_Record_Table *raw, Loan_Record_Table *filled, const uint col_index[], const uint size )
{
	TableColumnOperatorHelper< Optional< T >, Raw_Loan_Record_Table > raw_column( *raw );
	TableColumnOperatorHelper< int, Loan_Record_Table > filled_column( *filled );
	for ( uint i = 0; i < size; ++i )
	{
		auto &raw_col = raw_column[col_index[i]];
		auto &filled_col = filled_column[col_index[i]];
		int common_index, j = 0;
		ull max_cnt = 0;

		std::unordered_map< T, ull > countMap;
		std::unordered_map< T, int > classMap;
		for ( ull i = 0; i < raw_col.size; ++i )
		{
			if ( raw_col[i].hasValue() )
				++countMap[raw_col[i].value];
		}

		for ( const auto &[key, cnt] : countMap )
		{
			classMap[key] = j;
			if ( cnt > max_cnt )
			{
				max_cnt = cnt;
				common_index = j;
			}
			++j;
		}

		for ( ull j = 0; j < raw_column[i].size; ++j )
		{
			if ( !raw_col[j].hasValue() )
				filled_col.pushBack( common_index );
			else
				filled_col.pushBack( classMap[raw_col[j].value] );
		}
	}
}

template < typename T >
void grade_fill( Raw_Loan_Record_Table *raw, Loan_Record_Table *filled, const uint col_index[], const uint size, const uint grade = 3 )
{
	if ( grade <= 1 ) return;
	TableColumnOperatorHelper< Optional< T >, Raw_Loan_Record_Table > raw_column( *raw );
	TableColumnOperatorHelper< int, Loan_Record_Table > filled_column( *filled );

	T gradeStep, gradeMin;
	bool isGradeEven = ( grade % 2 == 0 );
	for ( uint i = 0; i < size; ++i )
	{
		auto &raw_col = raw_column[col_index[i]];
		auto &filled_col = filled_column[col_index[i]];
		gradeMin = raw_col.min;
		gradeStep = ( raw_col.max - gradeMin ) / grade;
		for ( ull j = 0; j < raw_column[i].size; ++j )
		{
			if ( !raw_col[j].hasValue() )
				// 或可换为随机值/随机+1/0
				filled_col.pushBack( grade / 2 + ( isGradeEven ? 0 : j % 2 ) );
			else
				filled_col.pushBack( int( ( raw_col[j].value - gradeMin - 1e-6 ) / gradeStep ) );
		}
	}
}

Loan_Record_Table *fill_and_class( Raw_Loan_Record_Table *raw )
{
	Loan_Record_Table *filled = new Loan_Record_Table( true, raw->dataRow );
	filled->header = new std::unordered_map< string, uint >( *raw->header );
	filled->header->erase( "issue_date" );
	filled->header->insert( { "issue_date_year", 12 } );
	filled->header->insert( { "issue_date_month", 39 } );
	filled->header->insert( { "issue_date_day", 40 } );


	avg_fill< int >( raw, filled, int_col_index, sizeof( int_col_index ) / sizeof( uint ) );
	grade_fill< double >( raw, filled, double_col_index, sizeof( double_col_index ) / sizeof( uint ), 5 );
	common_fill_and_class< ll >( raw, filled, ll_col_index, sizeof( ll_col_index ) / sizeof( uint ) );
	common_fill< char >( raw, filled, char_col_index, sizeof( char_col_index ) / sizeof( uint ) );
	common_fill_and_class< string >( raw, filled, string_col_index, sizeof( string_col_index ) / sizeof( uint ) );
	common_fill< bool >( raw, filled, bool_col_index, sizeof( bool_col_index ) / sizeof( uint ) );


	// 处理日期(2017/01/02)
	Column< Optional< string > > raw_date_column = raw->data->template static_get< 12 >();
	int *year, *month, *day;
	ull *lack;
	year = new int[raw->dataRow];
	month = new int[raw->dataRow];
	day = new int[raw->dataRow];
	std::unordered_map< int, ull > yearCnt, monthCnt, dayCnt;
	lack = new ull[raw->dataRow];
	ull j = 0;
	int second_slash;
	string date;

	for ( ull i = 0; i < raw->dataRow; ++i )
	{
		if ( raw_date_column[i].hasValue() )
		{
			date = raw_date_column[i].value;
			second_slash = date.rfind( '/' );
			year[i] = std::stoi( date.substr( 0, 4 ) );
			month[i] = std::stoi( date.substr( 5, second_slash - 5 ) );
			day[i] = std::stoi( date.substr( second_slash + 1, date.length() - second_slash - 1 ) );
			++yearCnt[year[i]];
			++monthCnt[month[i]];
			++dayCnt[day[i]];
		}
		else
		{
			lack[j++] = i;
		}
	}
	if ( j > 0 )
	{
		int commonYear = common_value_in_map( yearCnt );
		int commonMonth = common_value_in_map( monthCnt );
		int commonDay = common_value_in_map( dayCnt );
		for ( ull i = 0; i < j; ++i )
		{
			year[lack[i]] = commonYear;
			month[lack[i]] = commonMonth;
			day[lack[i]] = commonDay;
		}
	}
	Column< int > &yearCol = filled->data->template static_get< 12 >(), &monthCol = filled->data->template static_get< 39 >(), &dayCol = filled->data->template static_get< 40 >();
	for ( ull i = 0; i < raw->dataRow; ++i )
	{
		yearCol.pushBack( year[i] );
		monthCol.pushBack( month[i] );
		dayCol.pushBack( day[i] );
	}
	delete[] year;
	delete[] month;
	delete[] day;
	delete[] lack;

	return filled;
}


SmoothBayesClassPos *smooth_bayesian_fit( const Loan_Record_Table &filled, const uint *property_col_index, const uint tag_index, const uint property_col_cnt, double &p_is_default, double &p_not_default )
{
	ull is_default_cnt = 1;
	Column< bool > &is_default_col = filled.data->template static_get< 38 >();
	for ( ull i = 0; i < filled.dataRow; ++i )
	{
		if ( is_default_col[i] )
			++is_default_cnt;
	}

	// Laplacian correction
	p_is_default = (double)is_default_cnt / ( filled.dataRow + 2 );
	p_not_default = 1 - p_is_default;
	--is_default_cnt;
	ull not_default_cnt = filled.dataRow - is_default_cnt;

	uint i = 0;
	SmoothBayesClassPos *p_property = new SmoothBayesClassPos[property_col_cnt];
	for ( ; i < property_col_cnt; ++i )
		p_property[i].init( 2 );
	i = 0;
	Column< bool > &tag_col = *static_cast< Column< bool > * >( filled.data->get( tag_index ) );
	filled.data->forEach(
	    [&i, &tag_col, is_default_cnt, not_default_cnt, &p_property, property_col_index, tag_index, property_col_cnt]( uint index, auto &col )
	    {
		    if ( index == tag_index || i >= property_col_cnt || index != property_col_index[i] ) return;
		    std::unordered_map< int, ull > countMap1, countMap0;
		    std::unordered_map< int, bool > countMap;
		    for ( ull j = 0; j < col.size; ++j )
		    {
			    tag_col[j] ? ++countMap1[int( col[j] )] : ++countMap0[int( col[j] )];
			    countMap[int( col[j] )] = true;
		    }
		    uint size = countMap.size();
		    for ( const auto &[key, cnt] : countMap1 )
			    p_property[i].set_pos( 1, key, (double)( cnt + 1 ) / ( is_default_cnt + size ) );
		    for ( const auto &[key, cnt] : countMap0 )
			    p_property[i].set_pos( 0, key, (double)( cnt + 1 ) / ( not_default_cnt + size ) );
		    double *default_p = new double[2]{ 1.0 / ( not_default_cnt + size ), 1.0 / ( is_default_cnt + size ) };
		    p_property[i].set_default_pos( default_p );
		    delete[] default_p;
		    ++i;
	    } );
	return p_property;
}

bool *smooth_bayesian_guess( Loan_Record_Table &filled, const SmoothBayesClassPos *p_property, const uint *property_col_index, const uint property_col_cnt, const double p_is_default, const double p_not_default )
{
	bool *guess_result = new bool[filled.dataRow];
	double l_p_is_default, l_p_not_default;
	for ( ull i = 0; i < filled.dataRow; ++i )
	{
		Loan_Reacord l = filled.row< Loan_Reacord >( i );
		l_p_is_default = p_is_default;
		l_p_not_default = p_not_default;
		for ( uint i = 0; i < property_col_cnt; ++i )
		{
			l_p_not_default *= p_property[i].get_pos( 0, l.data[property_col_index[i]] );
			l_p_is_default *= p_property[i].get_pos( 1, l.data[property_col_index[i]] );
		}
		// todo
		guess_result[i] = l_p_is_default > l_p_not_default;
	}
	return guess_result;
}

int main()
{
	Raw_Loan_Record_Table *raw = Raw_Loan_Record_Table::FromCSV( "./train.csv" );
	Loan_Record_Table *filled_p = fill_and_class( raw );
	Loan_Record_Table &filled = *filled_p;
	std::cout << filled.toCSV( "./filled.csv" ) << std::endl;
	// 属性列
	uint *property_col_index = new uint[40];
	for ( uint i = 0; i < 38; ++i )
		property_col_index[i] = i;
	property_col_index[38] = 39;
	property_col_index[39] = 40;
	double p_is_default, p_not_default;


	// 计算每个属性的条件概率
	SmoothBayesClassPos *p_property = smooth_bayesian_fit( filled, property_col_index, 38, 40, p_is_default, p_not_default );


	Raw_Loan_Record_Table *test_raw = Raw_Loan_Record_Table::FromCSV( "./test.csv" );
	Loan_Record_Table *test_filled_p = fill_and_class( test_raw );
	Loan_Record_Table &test_filled = *test_filled_p;
	std::cout << test_filled.toCSV( "./test_filled.csv" ) << std::endl;

	bool *guess_result = smooth_bayesian_guess( test_filled, p_property, property_col_index, 40, p_is_default, p_not_default );
	Column< bool > &is_default_col = test_filled.data->template static_get< 38 >();
	ull j = 0;
	for ( ull i = 0; i < test_filled.dataRow; ++i )
	{
		if ( is_default_col[i] == guess_result[i] )
			++j;
	}
	std::cout << "accuracy: " << (double)j / test_filled.dataRow << std::endl;

	Column< bool > *guess_col = new Column< bool >( guess_result, test_filled.dataRow );
	Raw_Loan_Record_Table_And_Guess *result = test_raw->appendColumn< bool >( "guess", *guess_col );
	std::cout << result->toCSV( "./result.csv" ) << std::endl;
	delete result;
	delete guess_col;
	delete[] guess_result;
	delete test_raw;
	delete test_filled_p;

	delete[] p_property;
	delete[] property_col_index;
	delete filled_p;
	delete raw;
	return 0;
}

/*
0 1 2 3 4
1,
1.0,
5919.230769,
16.24615385,
0,
0,
Nov-85,
0,
1,
4.0,
0.0,
6.0,
10.0,
5.0,
2,73,
21.9,
0
*/