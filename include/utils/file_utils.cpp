#pragma once

#include "../base.hpp"
#include <algorithm>
#include <fstream>
#include <string>


namespace file_utils
{

	ull count_lines( std::ifstream &file, const std::string &filename, const char endline = '\n' )
	{
		if ( !file.is_open() )
		{
			throw std::runtime_error( "Cannot open file:" + filename );
		}

		// 统计 '\n'
		ull lines = std::count(
		    std::istreambuf_iterator< char >( file ),
		    std::istreambuf_iterator< char >(),
		    endline );

		file.clear();
		file.seekg( 0, std::ios::end );  // 判断文件末尾是否有最后一行没有 '\n'
		if ( file.tellg() > 0 )
		{
			file.seekg( -1, std::ios::end );
			char lastChar;
			file.get( lastChar );
			if ( lastChar != endline ) ++lines;
		}
		file.seekg( 0 );

		return lines;
	}
	ull count_items( std::ifstream &file, const std::string &filename, const char delimiter = ',' )
	{
		if ( !file.is_open() )
		{
			throw std::runtime_error( "Cannot open file:" + filename );
		}
		ull items = 0;
		std::string line;
		while ( std::getline( file, line ) )
			items += std::count( line.begin(), line.end(), delimiter ) + 1;
		file.clear();
		file.seekg( 0 );
		return items;
	}
	ull count_lines( const std::string &filename, const char endline = '\n' )
	{
		std::ifstream file( filename, std::ios::binary );  // 二进制模式
		return count_lines( file, filename, endline );
	}
	ull count_items( const std::string &filename, const char delimiter = ',' )
	{
		std::ifstream file( filename, std::ios::binary ); 
		return count_items( file, filename, delimiter );
	}
}