[[deprecated]]
#include <cstdio>
#include <cstring>
class Table
{
	typedef unsigned int uint;
	uint colsize, rowsize;
	const char **colnames;
	char **data;
public:
	Table( uint colsize, uint rowsize, const char **colnames, const char ***data ) : colsize( colsize ), rowsize( rowsize )
	{
		this->colnames = new const char *[colsize];
		this->data = new char *[rowsize * ( colsize - 1 )];
		for ( uint i = 0; i < colsize; ++i )
		{
			this->colnames[i] = strdup( colnames[i] );
			for ( uint j = 0; j < rowsize; ++j )
				this->data[i * rowsize + j] = strdup( data[i][j] );
		}
	}
	~Table()
	{
		for ( uint i = 0; i < rowsize * ( colsize - 1 ); ++i )
			delete[] data[i];
		delete[] colnames;
		delete[] data;
	}
	static Table *fromCSV( const char *filename )
	{
		FILE *fp = fopen( filename, "r" );
		if ( !fp )
		{
			// error
			return nullptr;
		}
	}
};

#include <cstdlib>  // malloc, free
#include <cstring>  // strcpy
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

class CSVReaderFixed
{
private:
	size_t maxRows;
	size_t maxCols;
	char ***data;   // 保存每个单元格的字符串
	char **header;  // 表头
	size_t rowsRead;
	size_t colsRead;
public:
	CSVReaderFixed( const std::string &filename, size_t maxRows_, size_t maxCols_, bool hasHeader = true, char delimiter = ',' )
	    : maxRows( maxRows_ ), maxCols( maxCols_ ), rowsRead( 0 ), colsRead( 0 )
	{
		// 分配表头数组
		header = (char **)malloc( maxCols * sizeof( char * ) );
		if ( !header ) throw std::bad_alloc();

		// 分配数据数组
		data = (char ***)malloc( maxRows * sizeof( char ** ) );
		if ( !data ) throw std::bad_alloc();
		for ( size_t i = 0; i < maxRows; i++ )
		{
			data[i] = (char **)malloc( maxCols * sizeof( char * ) );
			if ( !data[i] ) throw std::bad_alloc();
			for ( size_t j = 0; j < maxCols; j++ )
				data[i][j] = nullptr;  // 初始化为空
		}

		std::ifstream file( filename );
		if ( !file.is_open() )
		{
			throw std::runtime_error( "无法打开文件: " + filename );
		}

		std::string line;
		bool firstLine = true;

		while ( std::getline( file, line ) && rowsRead < maxRows )
		{
			std::stringstream ss( line );
			std::string cell;
			size_t col = 0;

			while ( std::getline( ss, cell, delimiter ) && col < maxCols )
			{
				char *cellStr = (char *)malloc( cell.size() + 1 );
				if ( !cellStr ) throw std::bad_alloc();
				std::strcpy( cellStr, cell.c_str() );

				if ( firstLine && hasHeader )
				{
					header[col++] = cellStr;
				}
				else
				{
					data[rowsRead][col++] = cellStr;
				}
			}

			if ( firstLine && hasHeader )
			{
				colsRead = col;
				firstLine = false;
			}
			else
			{
				if ( col > colsRead ) colsRead = col;
				rowsRead++;
			}
		}
	}

	~CSVReaderFixed()
	{
		// 释放表头
		for ( size_t j = 0; j < colsRead; j++ )
		{
			if ( header[j] ) free( header[j] );
		}
		free( header );

		// 释放数据
		for ( size_t i = 0; i < rowsRead; i++ )
		{
			for ( size_t j = 0; j < colsRead; j++ )
			{
				if ( data[i][j] ) free( data[i][j] );
			}
			free( data[i] );
		}
		free( data );
	}

	char *getCell( size_t row, size_t col ) const
	{
		if ( row >= rowsRead || col >= colsRead )
			throw std::out_of_range( "CSV索引越界" );
		return data[row][col];
	}

	char *getHeader( size_t col ) const
	{
		if ( col >= colsRead ) throw std::out_of_range( "CSV表头越界" );
		return header[col];
	}

	size_t numRows() const { return rowsRead; }
	size_t numCols() const { return colsRead; }
};
