#pragma once
// (some of
#include "tensor.cc"
#include <cmath>
// todo move to initializer
template < typename T >
Tensor< T > Tensor< T >::Identity( const uint n )
{
	Tensor< T > I( { n, n } );
	for ( uint i = 0; i < n * n; ++i )
		I.r_data[i] =  // i % n == i / n ? 1 : 0;
		    0;
	for ( uint i = 0; i < n; ++i )
		I.r_data[i * n + i] = 1;
	return I;
}
// if need the back of rearranged,copy one first
// ment[i] means the new arranged i-th row is the orig ment[i]-th row
template < typename T >
Tensor< uint > Tensor< T >::Max_I_I_Rearrange( const Tensor< T > &A, Tensor< T > *rearrange_in_p )
{
	if ( A.shape.dim > 2 )
		throw std::runtime_error( "Tensor Pivot: A 's dim must <= 2" );
	Tensor< T > *rearrange_p = rearrange_in_p ? rearrange_in_p : new Tensor< T >( A.copy() );
	uint *oriar = new uint[A.shape.dimsSize[0]];
	for ( uint i = 0; i < A.shape.dimsSize[0]; ++i )
		oriar[i] = i;
	Tensor< uint > ment( { A.shape.dimsSize[0] }, oriar );
	delete[] oriar;
	uint max_i, tmpar;
	T cur, tmp;
	for ( uint i = 0; i < A.shape.dimsSize[0]; ++i )
	{
		max_i = i;
		cur = abs( rearrange_p->r_data[rearrange_p->shape.offset + max_i * rearrange_p->shape.stride[0] + i * rearrange_p->shape.stride[1]] );
		for ( uint j = i + 1; j < A.shape.dimsSize[0]; ++j )
		{
			tmp = abs( rearrange_p->r_data[rearrange_p->shape.offset + j * rearrange_p->shape.stride[0] + i * rearrange_p->shape.stride[1]] );
			if ( tmp > cur )
			{
				cur = tmp;
				max_i = j;
			}
		}
		if ( cur <= TENSOR_SCALAR_ZERO )
			throw std::runtime_error( "Tensor Max_I_I_Rearrange: faile with [" + std::to_string( cur ) + "] too small" );
		if ( max_i != i )
		{
			for ( uint j = 0; j < A.shape.dimsSize[1]; j++ )
			{
				tmp = rearrange_p->r_data[rearrange_p->shape.offset + i * rearrange_p->shape.stride[0] + j * rearrange_p->shape.stride[1]];
				rearrange_p->r_data[rearrange_p->shape.offset + i * rearrange_p->shape.stride[0] + j * rearrange_p->shape.stride[1]] =
				    rearrange_p->r_data[rearrange_p->shape.offset + max_i * rearrange_p->shape.stride[0] + j * rearrange_p->shape.stride[1]];
				rearrange_p->r_data[rearrange_p->shape.offset + max_i * rearrange_p->shape.stride[0] + j * rearrange_p->shape.stride[1]] = tmp;
			}
			tmpar = ment.r_data[i];
			ment.r_data[i] = ment.r_data[max_i];
			ment.r_data[max_i] = tmpar;
		}
	}
	if ( !rearrange_in_p )
		delete rearrange_p;
	return ment;
}

template < typename T >
void Tensor< T >::LU_of( const Tensor< T > &A, Tensor< T > &L, Tensor< T > &U, const Tensor< uint > &ra )
{
	if ( A.shape.dim != 2 )
		throw std::runtime_error( "Tensor LU_of: A 's dim must = 2" );
	uint n = A.shape.dimsSize[0];
	if ( n != A.shape.dimsSize[1] )
		throw std::runtime_error( "Tensor LU_of: A must be a square matrix" );
	for ( uint i = 0; i < n * n; ++i )
		L.r_data[i] =  // ra.data( i % n ) == i / n ? 1 : 0;
		    0;
	for ( uint i = 0, j = ra.data( 0 ); i < n; ++i, j = ra.data( i ) )
	{
		L.r_data[i * n + i] = 1;
		for ( uint k = 0; k < n; ++k )
			U.r_data[i * n + k] = A.data( j, k );
	}
	ull index;
	for ( uint i = 1; i < n; ++i )
		for ( uint j = 0; j < i; ++j )
		{
			index = i * n + j;
			L.r_data[index] = U.r_data[index] / U.r_data[j * n + j];
			for ( uint k = 0; k < n; ++k )
				U.r_data[i * n + k] -= L.r_data[index] * U.r_data[j * n + k];
		}
}

template < typename T >
Tensor< T > Tensor< T >::Solve_LU( const Tensor< T > &L, const Tensor< T > &U, const Tensor< T > &b, const Tensor< uint > &ra )
{
	if ( L.shape.dim > 2 || U.shape.dim > 2 || b.shape.dim > 2 )
		throw std::runtime_error( "Tensor Solve_LU: LU and b 's dim must <= 2" );
	if ( L.shape.dimsSize[0] != L.shape.dimsSize[1] )
		throw std::runtime_error( "Tensor Solve_LU: L must be a square matrix" );
	if ( U.shape.dimsSize[0] != U.shape.dimsSize[1] )
		throw std::runtime_error( "Tensor Solve_LU: U must be a square matrix" );
	if ( L.shape.dimsSize[0] != U.shape.dimsSize[0] )
		throw std::runtime_error( "Tensor Solve_LU: L and U must have the same row size" );
	if ( b.shape.dim == 1 )
	{
		if ( b.shape.dimsSize[0] != L.shape.dimsSize[0] )
			throw std::runtime_error( "Tensor Solve_LU: vector b's size must equal to A's row size" );
		Tensor< T > y( { b.shape.dimsSize[0] } );
		for ( uint i = 0; i < b.shape.dimsSize[0]; ++i )
		{
			y.r_data[i] = b.data( ra.data( i ) );
			for ( uint j = 0; j < i; ++j )
				y.r_data[i] -= L.data( i, j ) * y.r_data[j];
		}
		Tensor< T > x( { b.shape.dimsSize[0] } );
		for ( uint i = b.shape.dimsSize[0] - 1; i > 0; --i )
		{
			x.r_data[i] = y.r_data[i];
			for ( uint j = i + 1; j < b.shape.dimsSize[0]; ++j )
				x.r_data[i] -= U.data( i, j ) * x.r_data[j];
			x.r_data[i] /= U.data( i, i );
		}
		x.r_data[0] = y.r_data[0];
		for ( uint j = 1; j < b.shape.dimsSize[0]; ++j )
			x.r_data[0] -= U.data( 0, j ) * x.r_data[j];
		x.r_data[0] /= U.data( 0, 0 );
		return x;
	}
	else
	{
		if ( b.shape.dimsSize[1] != L.shape.dimsSize[0] )
			throw std::runtime_error( "Tensor Solve_LU: matirx b's col size must equal to A's row size" );
		Tensor< T > X( { b.shape.dimsSize[0], b.shape.dimsSize[1] } );
		for ( uint i = 0; i < b.shape.dimsSize[1]; ++i )
			X[i] = Tensor< T >::Solve_LU( L, U, b[i], ra );
		return X;
	}
}
template < typename T >
Tensor< T > Tensor< T >::oneHot( const uint &size ) const
{
	Tensor< T > t( { this->shape.dimsSize[0], size } );
	uint j;
	for ( uint i = 0; i < this->shape.dimsSize[0]; ++i )
	{
		for ( j = 0; j < size; ++j )
			if ( j == uint( this->r_data[this->shape.offset + i * this->shape.stride[0]] ) )
			{
				t.r_data[i * size + j] = 1;
				break;
			}
			else
				t.r_data[i * size + j] = 0;
		for ( ++j; j < size; ++j )
			t.r_data[i * size + j] = 0;
	}
	return t;
}
template < typename T >
Tensor< T > Tensor< T >::Exp( const Tensor< T > &self )
{
	Tensor< T > t( TensorShape( self.shape.dimsSize, self.shape.dim ) );
	ull index, k;
	uint j;
	uint dim = self.shape.dim;
	ull *tstride = self.shape.stride;
	ull *stride = t.shape.stride;
	for ( ull i = 0; i < t.size; ++i )
	{
		k = i;
		index = self.shape.offset;
		for ( j = 0; j < dim; ++j )
		{
			index += k / stride[j] * tstride[j];
			k %= stride[j];
		}
		t.r_data[i] = T( std::exp( self.r_data[index] ) );
	}
	return t;
}
template < typename T >
Tensor< T > Tensor< T >::Ln( const Tensor< T > &self )
{
	Tensor< double > t( TensorShape( self.shape.dimsSize, self.shape.dim ) );
	ull index, k;
	uint j;
	uint dim = self.shape.dim;
	ull *tstride = self.shape.stride;
	ull *stride = t.shape.stride;
	for ( ull i = 0; i < t.size; ++i )
	{
		k = i;
		index = self.shape.offset;
		for ( j = 0; j < dim; ++j )
		{
			index += k / stride[j] * tstride[j];
			k %= stride[j];
		}
		t.r_data[i] = self.r_data[index] <= 0 ? T( -1000000 ) : T( std::log( self.r_data[index] ) );
	}
	return t;
}
template < typename T >
inline double sigmoid_scaler( const T &x )
{
	if ( x >= 0 )
	{
		double z = std::exp( -x );
		return 1.0 / ( 1.0 + z );
	}
	else
	{
		double z = std::exp( x );
		return z / ( 1.0 + z );
	}
}

template < typename T >
Tensor< T > Tensor< T >::sigmoid() const
{
	Tensor< T > t( TensorShape( this->shape.dimsSize, this->shape.dim ) );
	ull index, k;
	uint j;
	uint dim = this->shape.dim;
	ull *tstride = this->shape.stride;
	ull *stride = t.shape.stride;
	for ( ull i = 0; i < t.size; ++i )
	{
		k = i;
		index = this->shape.offset;
		for ( j = 0; j < dim; ++j )
		{
			index += k / stride[j] * tstride[j];
			k %= stride[j];
		}
		t.r_data[i] = T( sigmoid_scaler< T >( this->r_data[index] ) );
	}
	return t;
}
template < typename T >
Tensor< T > Tensor< T >::ReLU() const
{
	Tensor< T > t( TensorShape( this->shape.dimsSize, this->shape.dim ) );
	ull index, k;
	uint j;
	uint dim = this->shape.dim;
	ull *tstride = this->shape.stride;
	ull *stride = t.shape.stride;
	for ( ull i = 0; i < t.size; ++i )
	{
		k = i;
		index = this->shape.offset;
		for ( j = 0; j < dim; ++j )
		{
			index += k / stride[j] * tstride[j];
			k %= stride[j];
		}
		t.r_data[i] = ( this->r_data[index] > 0 ) ? this->r_data[index] : 0;
	}
	return t;
}
#include "../utils/file_utils.cpp"
#include <sstream>
template < typename T >
Tensor< T > Tensor< T >::FromCSV( const std::string &filename, bool hasHeader, char delimiter, char endline )
{
	std::ifstream file( filename );
	if ( !file.is_open() )
		throw std::runtime_error( "Tensor FromCSV: Could not open file: " + filename );
	const uint row = file_utils::count_lines( file, filename, endline );
	std::string line;
	std::getline( file, line );
	const uint col = std::count( line.begin(), line.end(), delimiter ) + 1;
	Tensor< T > t( { row - hasHeader, col } );
	ull index = 0;
	if ( !hasHeader )
	{
		std::stringstream ss( line );
		while ( std::getline( ss, line, delimiter ) )
			t.r_data[index++] = sto< T >( line );
	}
	while ( std::getline( file, line ) )
	{
		std::stringstream ss( line );
		while ( std::getline( ss, line, delimiter ) )
			t.r_data[index++] = sto< T >( line );
	}
	file.close();
	return t;
}
template <>
Tensor< std::string > Tensor< std::string >::FromCSV( const std::string &filename, bool hasHeader, char delimiter, char endline )
{
	std::ifstream file( filename );
	if ( !file.is_open() )
		throw std::runtime_error( "Tensor FromCSV: Could not open file: " + filename );
	const uint row = file_utils::count_lines( file, filename, endline );
	std::string line;
	std::getline( file, line );
	const uint col = std::count( line.begin(), line.end(), delimiter ) + 1;
	Tensor< std::string > t( { row, col } );
	ull index = 0;
	if ( !hasHeader )
	{
		std::stringstream ss( line );
		while ( std::getline( ss, line, delimiter ) )
			t.r_data[index++] = line;
	}
	while ( std::getline( file, line ) )
	{
		std::stringstream ss( line );
		while ( std::getline( ss, line, delimiter ) )
			t.r_data[index++] = line;
	}
	file.close();
	return t;
}