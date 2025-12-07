#pragma once

#include "../base.hpp"
#include <algorithm>
#include <fstream>
#include <string>


namespace file_utils
{

	uint count_lines( std::ifstream &file, const std::string &filename )
	{
		if ( !file.is_open() )
		{
			throw std::runtime_error( "Cannot open file:" + filename );
		}

		// 统计 '\n'
		uint lines = std::count(
		    std::istreambuf_iterator< char >( file ),
		    std::istreambuf_iterator< char >(),
		    '\n' );

		file.clear();
		file.seekg( 0, std::ios::end );  // 判断文件末尾是否有最后一行没有 '\n'
		if ( file.tellg() > 0 )
		{
			file.seekg( -1, std::ios::end );
			char lastChar;
			file.get( lastChar );
			if ( lastChar != '\n' ) ++lines;
		}
		file.seekg( 0 );
		
		return lines;
	}

	uint count_lines( const std::string &filename )
	{
		std::ifstream file( filename, std::ios::binary );  // 二进制模式
		return count_lines( file, filename );
	}

}