#pragma once
#include <iostream>
#include <unordered_map>
struct SmoothBayesClassPos
{
	std::unordered_map< int, double > *class_pos = nullptr;
	double *default_pos = nullptr;
	unsigned int class_num;
	SmoothBayesClassPos() {}
	SmoothBayesClassPos( unsigned int class_num ) : class_num( class_num )
	{
		class_pos = new std::unordered_map< int, double >[class_num];
		default_pos = new double[class_num];
	}
	void init( unsigned int class_num )
	{
		this->class_num = class_num;
		class_pos = new std::unordered_map< int, double >[class_num];
		default_pos = new double[class_num];
	}
	~SmoothBayesClassPos()
	{
		delete[] class_pos;
		delete[] default_pos;
	}

	void set_default_pos( double *pos )
	{
		for ( unsigned int i = 0; i < class_num; ++i )
			default_pos[i] = pos[i];
	}
	void set_pos( unsigned int class_id, int property_class_id, double pos )
	{
		class_pos[class_id][property_class_id] = pos;
	}
	double get_pos( unsigned int class_id, int property_class_id ) const
	{
		if ( class_pos[class_id].find( property_class_id ) != class_pos[class_id].end() )
			return class_pos[class_id][property_class_id];
		return default_pos[class_id];
	}
};