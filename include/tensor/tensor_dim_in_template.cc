[[deprecated]]
#pragma once
#include <iostream>

#define 𤳳 Tensor
typedef unsigned int uint;
typedef unsigned long long ull;
struct TensorShape
{
	uint dim = 0;
	ull size = 1;
	ull offset = 0;
	uint *dimsSize = nullptr;
	ull *stride = nullptr;
	// template < ull N = 1 >
	// TensorShape( const uint ( &dimsSize )[N] )
	// {
	// 	this->dimsSize = new uint[N];
	// 	for ( ull i = 0; i < N; ++i )
	// 		this->dimsSize[i] = dimsSize[i];
	// }
	TensorShape( const uint *dimsSize, uint dim = 0 )
	{
		if ( dim == 0 )
		{
			this->dimsSize = new uint[1];
			this->dimsSize[0] = 0;
		}
		// else if ( dim < dimsSizeSize )
		// 	throw std::runtime_error( "Tensor Shape: dimsSize more than dim " );
		else
		{
			this->dim = dim;
			this->dimsSize = new uint[dim];
			this->stride = new ull[dim];
			// dimsSizeSize = ( dimsSizeSize ? dimsSizeSize : dim ) - 1;
			this->stride[dim - 1] = 1;
			for ( uint i = dim - 1; i > 0; --i )
			{
				this->dimsSize[i] = dimsSize[i];
				this->stride[i - 1] = this->stride[i] * this->dimsSize[i];
			}
			this->dimsSize[0] = dimsSize[0];
			this->size = this->stride[0] * dimsSize[0];
		}
	}
	TensorShape( const TensorShape &t ) : dim( t.dim ), size( t.size ), offset( t.offset )
	{
		if ( dim )
		{
			this->stride = new ull[dim];
			this->dimsSize = new uint[dim];
			for ( uint i = 0; i < dim; ++i )
			{
				this->dimsSize[i] = t.dimsSize[i];
				this->stride[i] = t.stride[i];
			}
		}
		else
		{
			this->dimsSize = new uint[1];
			this->dimsSize[0] = 0;
		}
	}
	TensorShape( TensorShape &&t ) : dim( t.dim ), size( t.size ), offset( t.offset ), dimsSize( t.dimsSize ), stride( t.stride )
	{
		t.dimsSize = nullptr;
		t.stride = nullptr;
	}
	TensorShape &operator=( const TensorShape &t )
	{
		if ( this != &t )
		{
			delete[] this->dimsSize;
			delete[] this->stride;
			this->dim = t.dim;
			if ( dim )
			{
				this->size = t.size;
				this->offset = t.offset;
				this->dimsSize = new uint[t.dim];
				this->stride = new ull[t.dim];
				for ( uint i = 0; i < t.dim; ++i )
				{
					this->dimsSize[i] = t.dimsSize[i];
					this->stride[i] = t.stride[i];
				}
			}
			else
			{
				this->dimsSize = new uint[1];
				this->dimsSize[0] = 0;
			}
		}
		return *this;
	}
	TensorShape &operator=( TensorShape &&t )
	{
		if ( this != &t )
		{
			delete[] this->dimsSize;
			delete[] this->stride;
			this->dim = t.dim;
			this->size = t.size;
			this->offset = t.offset;
			this->dimsSize = t.dimsSize;
			this->stride = t.stride;
			t.dimsSize = nullptr;
			t.stride = nullptr;
		}
		return *this;
	}
	~TensorShape()
	{
		delete[] dimsSize;
		delete[] stride;
	}
	friend std::ostream &operator<<( std::ostream &os, const TensorShape &t );
};

std::ostream &operator<<( std::ostream &os, const TensorShape &t )
{
	os << "{\ndim:" << t.dim << ",\nsize:" << t.size << ",\noffset:" << t.offset << ",\ndimsSize:{";
	uint i;
	for ( i = 0; i < t.dim - 1; ++i )
		os << t.dimsSize[i] << ",";
	os << t.dimsSize[i] << "},\nstride:{";
	for ( i = 0; i < t.dim - 1; ++i )
		os << t.stride[i] << ",";
	os << t.stride[i] << "}\n}";
	return os;
}


template < typename T, uint N >
struct Tensor
{
	bool isnView = true;
	T *r_data;
	ull size = 0;
	TensorShape shape;
	Tensor() = default;
	Tensor( const uint ( &dimsSize )[N] = { 3, 2, 1 } ) : shape( dimsSize, N )
	{
		this->r_data = new T[shape.size];
		this->size = shape.size;
	}

	// get view
	Tensor( TensorShape &&shape, T *data, ull size ) : shape( std::move( shape ) ), r_data( data ), size( size ), isnView( false ) {}
	// Tensor( const TensorShape &shape,const uint  ) : shape( shape ) {}
	Tensor( const Tensor &t ) : shape( t.shape ), isnView( t.isnView ), size( t.size )
	{
		// 1. t is view ,i wanna get a view too
		// 2. t is not view ,i wanna get a view too  x
		// 3. t is view ,i wanna get a new tensor    x
		// 4. t is not view ,i wanna get a new tensor
		if ( t.isnView )
		{
			this->r_data = new T[t.size];
			for ( ull i = 0; i < t.size; ++i )
				this->r_data[i] = t.r_data[i];
		}
		else
			this->r_data = t.r_data;
	}
	Tensor( Tensor &&t ) : shape( std::move( t.shape ) ), isnView( t.isnView ), size( t.size )
	{
		this->r_data = t.r_data;
		t.r_data = nullptr;
	}
	Tensor &operator=( const Tensor &t )
	{
		if ( this != &t )
		{
			this->shape = t.shape;
			this->size = t.size;
			if ( this->isnView )
				delete[] r_data;
			this->isnView = t.isnView;
			if ( t.isnView )
			{
				this->r_data = new T[t.size];
				for ( ull i = 0; i < t.size; ++i )
					this->r_data[i] = t.r_data[i];
			}
			else
				this->r_data = t.r_data;
		}
		return *this;
	}
	Tensor &operator=( Tensor &&t )
	{
		if ( this != &t )
		{
			this->shape = std::move( t.shape );
			this->size = t.size;
			this->isnView = t.isnView;
			this->r_data = t.r_data;
			t.r_data = nullptr;
		}
		return *this;
	}
	// get view
	Tensor &operator()( uint operDim, const uint start = 0, uint end = 0, uint step = 1 )
	{
		if ( operDim == 0 || operDim > shape.dim )
			throw std::runtime_error( "Tensor: operDim out of Tensor" );
		if ( end >= shape.dimsSize[N - 1] )
			end = shape.dimsSize[N - 1] - 1;
		else if ( end == 0 )
		{
			end = start;
			step = 1;
		}
		if ( step == 0 )
			throw std::runtime_error( "Tensor: step can't be 0" );
		--operDim;
		// todo:check
		TensorShape newShape( this->shape );
		newShape.offset += start * this->shape.stride[operDim];
		newShape.dimsSize[operDim] = ( end - start ) / step + 1;
		newShape.stride[operDim] *= step;
		return *new Tensor( std::move( newShape ), this->r_data, this->size );
	}
	// get view
	Tensor< T, N - 1 > &operator[]( const uint i )
	{
		if ( i >= shape.dimsSize[0] )
			throw std::runtime_error( "Tensor: index out of range" );
		TensorShape newShape( this->shape.dimsSize + 1, N - 1 );
		newShape.offset = this->shape.offset + i * this->shape.stride[0];
		return *new Tensor< T, N - 1 >( std::move( newShape ), this->r_data, this->size );
	}
	Tensor &copy()
	{
		bool isnView = this->isnView;
		this->isnView = true;
		Tensor *t = new Tensor( *this );
		this->isnView = isnView;
		return *t;
	}
	template < uint M >

	Tensor< T, M > &reshape( const uint ( &dimsSize )[M] )
	{
		TensorShape newShape( dimsSize, M );
		if ( this->shape.size != newShape.size )
			throw std::runtime_error( "Tensor: reshape size not match" );
		return *new Tensor< T, M >( std::move( newShape ), this->r_data, this->size );
	}
	T &dataOper( const uint ( &a )[N] )
	{
		uint index = shape.offset;
		for ( uint i = 0; i < N; ++i )
			index += a[i] * shape.stride[i];
		return r_data[index];
	}
	~Tensor()
	{
		if ( isnView )
			delete[] r_data;
	}
};



template < typename T >
struct Tensor< T, 1 >
{
	bool isnView = true;
	T *r_data;
	ull size = 0;
	TensorShape shape;
	Tensor( TensorShape &&shape, T *data, ull size ) : shape( std::move( shape ) ), r_data( data ), size( size ), isnView( false ) {}
	Tensor( const Tensor &t ) : shape( t.shape ), isnView( t.isnView ), size( t.size )
	{
		if ( t.isnView )
		{
			this->r_data = new T[t.size];
			for ( ull i = 0; i < t.size; ++i )
				this->r_data[i] = t.r_data[i];
		}
		else
			this->r_data = t.r_data;
	}
	Tensor( Tensor &&t ) : shape( std::move( t.shape ) ), isnView( t.isnView ), size( t.size )
	{
		this->r_data = t.r_data;
		t.r_data = nullptr;
	}
	Tensor &operator=( const Tensor &t )
	{
		if ( this != &t )
		{
			this->shape = t.shape;
			this->size = t.size;
			if ( this->isnView )
				delete[] r_data;
			this->isnView = t.isnView;
			if ( t.isnView )
			{
				this->r_data = new T[t.size];
				for ( ull i = 0; i < t.size; ++i )
					this->r_data[i] = t.r_data[i];
			}
			else
				this->r_data = t.r_data;
		}
		return *this;
	}
	Tensor &operator=( Tensor &&t )
	{
		if ( this != &t )
		{
			this->shape = std::move( t.shape );
			this->size = t.size;
			this->isnView = t.isnView;
			this->r_data = t.r_data;
			t.r_data = nullptr;
		}
		return *this;
	}
	T &operator[]( const uint i )
	{
		if ( i >= shape.dimsSize[0] )
			throw std::runtime_error( "Tensor: index out of range" );
		return this->r_data[shape.offset + i * shape.stride[0]];
	}
	template < uint M >

	Tensor< T, M > &reshape( const uint ( &dimsSize )[M] )
	{
		TensorShape newShape( dimsSize, M );
		if ( this->shape.size != newShape.size )
			throw std::runtime_error( "Tensor: reshape size not match" );
		return *new Tensor< T, M >( std::move( newShape ), this->r_data, this->size );
	}
	T &dataOper( const uint ( &a )[1] )
	{
		return r_data[shape.offset + a[0] * shape.stride[0]];
	}
	~Tensor()
	{
		if ( isnView )
			delete[] r_data;
	}
};


#define data( a... ) dataOper( { a } )

// int main( int argc, char const *argv[] )
// {
// 	// TensorShape< 2 > shape( { 1, 2 } );
// 	Tensor< int, 3 > t( { 2, 3, 4 } );
// 	int a = 0;

// 	t.data( 1, 1, 1 );
// 	std::cout << t.shape << std::endl;
// 	for ( uint i = 0; i < 2; ++i )
// 		for ( uint j = 0; j < 3; ++j )
// 			for ( uint k = 0; k < 4; ++k )
// 				t[i][j][k] = ++a;
// 	for ( uint i = 0; i < 2; ++i )
// 	{
// 		for ( uint j = 0; j < 3; ++j )
// 		{
// 			for ( uint k = 0; k < 4; ++k )
// 				std::cout << t[i][j][k] << " ";
// 			std::cout << std::endl;
// 		}

// 		std::cout << std::endl
// 		          << std::endl;
// 	}
// 	for ( uint i = 0; i < 3; ++i )
// 	{
// 		for ( uint j = 0; j < 4; ++j )
// 			std::cout << t[0][i][j] << " ";
// 		std::cout << std::endl;
// 	}

// 	// t[0] = 1;
// 	// std::cout << t[0] << std::endl;
// 	return 0;
// }
