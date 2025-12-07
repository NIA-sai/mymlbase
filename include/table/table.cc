#include "../base.hpp"
#include "column.cc"
#include "../utils/file_utils.cpp"
#include "../utils/tuple.cc"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>



template < typename... ColumnTypes >
struct Table
{
	typedef std::string string;
	bool hasHeader = false;
	std::unordered_map< string, uint > *header = nullptr;
	Tuple< Column< ColumnTypes >... > *data = nullptr;
	Table() {}
	~Table()
	{
		delete header;
		delete data;
	}
	void *operator[]( uint columnIndex )
	{
		return data->get( columnIndex );
	}
	void *operator[]( string columnName )
	{
		if ( !hasHeader )
			throw std::runtime_error( "No header found" );
		return this->operator[]( header->at( columnName ) );
	}


	static Table< ColumnTypes... > &FromCSV( const string &filename, bool hasHeader = true, char delimiter = ',' )
	{
		Table< ColumnTypes... > *table_p = new Table< ColumnTypes... >();
		Table< ColumnTypes... > &table = *table_p;
		table.hasHeader = hasHeader;
		std::ifstream file( filename );
		if ( !file.is_open() )
			throw std::runtime_error( "Could not open file: " + filename );
		uint row = file_utils::count_lines( file, filename );
		string line;
		if ( hasHeader )
		{
			--row;
			table.header = new std::unordered_map< string, uint >();
			std::getline( file, line );
			std::stringstream ss( line );
			uint col = 0;
			while ( std::getline( ss, line, delimiter ) )
			{
				table.header->insert( { line, col++ } );
			}
		}
		table.data = new Tuple< Column< ColumnTypes >... >(
		    Column< ColumnTypes >( row )... );
		Tuple< Column< ColumnTypes >... > &data = *( table.data );
		while ( std::getline( file, line ) )
		{
			std::stringstream ss( line );
			uint col = 0;
			data.forEach( [&col, &line, &ss, &delimiter]( uint, auto &x )
			              {
				               std::getline( ss, line, delimiter );
				               x.pushBack( line ); } );
		}
		return table;
	}

	template < uint rowNum, uint colNum >
	auto &cell()
	{
		return data->template static_get< colNum >()[rowNum];
	}
	template < class C >
	Column< C > &column( uint colNum )
	{
		return *( (Column< C > *)( this->operator[]( colNum ) ) );
	}
	template < class C >
	Column< C > &column( std::string colName )
	{
		return *( (Column< C > *)( this->operator[]( colName ) ) );
	}
	template < class R, ull... Is >
	R rowImpl( uint rowNum, std::index_sequence< Is... > )
	{
		return R( ( data->template static_get< Is >()[rowNum] )... );
	}

	template < class R >
	R row( uint rowNum )
	{
		return rowImpl< R >( rowNum, std::make_index_sequence< sizeof...( ColumnTypes ) >{} );
	}
	bool toCSV( const string &filename, char delimiter = ',', char endline = '\n' )
	{
		std::ofstream file( filename );
		if ( !file.is_open() )
			return false;
		if ( hasHeader )
		{
			for ( uint i = 0; i < sizeof...( ColumnTypes ) - 1; ++i )
			{
				for ( auto &x : *header )
					if ( x.second == i )
					{
						file << x.first << delimiter;
						header->erase( x.first );
						break;
					}
			}
			for ( auto &x : *header )
				file << x.first;
			file << endline;
		}
		for ( uint i = 0; i < data->template static_get< 0 >().size; ++i )
		{
			data->forEach(
			    [&file, &i, &delimiter, &endline]( int index, auto &x )
			    {
				    file << x[i];
				    if ( index != sizeof...( ColumnTypes ) - 1 ) file << delimiter;
			    } );
			file << endline;
		}
		file.close();
		return true;
	}
};

template < class T, class Table >
struct TableColumnOperatorHelper
{
	Table &table;
	TableColumnOperatorHelper( Table &t ) : table( t ) {}
	Column< T > &operator[]( uint columnIndex )
	{
		return *( (Column< T > *)table[columnIndex] );
	}
};
struct TT
{
	std::string &a;
	int &b;
	TT( std::string &a, int &b ) : a( a ), b( b ) {}
};

[[test]]
int main()
{
	Table< std::string, int > &table = Table< std::string, int >::FromCSV( "./tt.csv" );
	TableColumnOperatorHelper< std::string, Table< std::string, int > > tstr( table );
	// table.row< TT >( 0 );
	// table.column< std::string >( 0 );
	// table.cell< 0, 0 >();
	tstr[0][0] = "111";
	std::cout<<table.toCSV( "./tt1.csv" )<<std::endl;
	std::cout << tstr[0][0];

	// delete &table;


	// return 0;
}