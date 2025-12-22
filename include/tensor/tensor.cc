#pragma once
#include "../base.hpp"
#include "../defines.def"
// #include "../oper/oper.hpp"
#include "../utils/not.hpp"
#include "../utils/sto_func_map.cpp"
#include <iostream>
#define 𤳳 Tensor
#ifndef TENSOR_SCALAR_ZERO
	#define TENSOR_SCALAR_ZERO 1e-10
#endif

typedef unsigned int uint;
typedef unsigned long long ull;

template < typename T >
inline T abs( const T &x )
{
	return x < 0 ? -x : x;
}


struct TensorShape
{
	bool isnView = true;
	bool isnSlice = true;
	uint dim = 0;
	ull size = 0;
	ull offset = 0;
	uint *dimsSize = nullptr;
	ull *stride = nullptr;
	TensorShape() = default;
	TensorShape(
	    bool isnView,
	    uint dim,
	    ull size,
	    ull offset,
	    uint *dimsSize,
	    ull *stride ) : isnView( isnView ), dim( dim ), size( size ), offset( offset ), dimsSize( dimsSize ), stride( stride ) {}
	TensorShape( const uint *dimsSize, uint dim = 0 )
	{
		if ( dim == 0 )
		{
			this->size = 1;
			this->dimsSize = new uint[1];
			this->dimsSize[0] = 0;
		}
		else
		{
			this->dim = dim;
			this->dimsSize = new uint[dim];
			this->stride = new ull[dim];
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
	template < uint N >
	TensorShape( const uint ( &dimsSize )[N] ) : TensorShape( dimsSize, N ) {}
	TensorShape( const TensorShape &t ) : dim( t.dim ), size( t.size ), offset( t.offset ), isnView( t.isnView ), isnSlice( t.isnSlice )
	{
		if ( t.isnView )
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
			this->dimsSize = t.dimsSize;
			this->stride = t.stride;
		}
	}
	TensorShape( TensorShape &&t ) : dim( t.dim ), size( t.size ), offset( t.offset ), dimsSize( t.dimsSize ), stride( t.stride ), isnView( t.isnView ), isnSlice( t.isnSlice )
	{
		t.dimsSize = nullptr;
		t.stride = nullptr;
	}
	TensorShape &operator=( const TensorShape &t )
	{
		if ( this != &t )
		{
			this->dim = t.dim;
			this->size = t.size;
			this->offset = t.offset;
			this->isnView = t.isnView;
			this->isnSlice = t.isnSlice;
			if ( t.isnView )
			{
				delete[] this->dimsSize;
				delete[] this->stride;
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
				this->dimsSize = t.dimsSize;
				this->stride = t.stride;
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
			this->isnView = t.isnView;
			this->isnSlice = t.isnSlice;
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
	bool operator==( const TensorShape &t ) const
	{
		if ( dim != t.dim )
			return false;
		for ( uint i = 0; i < dim; ++i )
			if ( dimsSize[i] != t.dimsSize[i] )
				return false;
		return true;
	}
	bool operator!=( const TensorShape &t ) const { return !( *this == t ); }
	~TensorShape()
	{
		if ( isnView )
		{
			delete[] dimsSize;
			delete[] stride;
		}
	}
	bool isSubOf( const TensorShape &t ) const
	{
		if ( this->dim > t.dim )
			return false;
		uint os = t.dim - this->dim;
		for ( uint i = 0; i < this->dim; ++i )
			if ( this->dimsSize[i] != t.dimsSize[os + i] )
				return false;
		return true;
	}
	friend std::ostream &operator<<( std::ostream &os, const TensorShape &t );
};

std::ostream &operator<<( std::ostream &os, const TensorShape &t )
{
	os << "{\n\tdim:" << t.dim << ",\n\tsize:" << t.size << ",\n\toffset:" << t.offset;
	if ( t.dim )
	{
		os << ",\n\tdimsSize:{";
		uint i;
		for ( i = 0; i < t.dim - 1; ++i )
			os << t.dimsSize[i] << ",";
		os << t.dimsSize[i] << "},\n\tstride:{";
		for ( i = 0; i < t.dim - 1; ++i )
			os << t.stride[i] << ",";
		os << t.stride[i] << "}";
	}
	os << "\n}";
	return os;
}

template < typename T >
struct Tensor
{
	T *r_data = nullptr;
	// really size
	ull size = 0;
	TensorShape shape;


	Tensor() = default;

	Tensor( const TensorShape &shape, const T *r_data = nullptr ) : shape( shape )
	{
		this->r_data = new T[shape.size];
		this->size = shape.size;
		if ( r_data )
		{
			for ( ull i = 0; i < shape.size; ++i )
				this->r_data[i] = r_data[i];
		}
	}
	Tensor( TensorShape &&shape, const T *r_data ) : shape( std::move( shape ) )
	{
		this->r_data = new T[shape.size];
		this->size = shape.size;
		if ( r_data )
		{
			for ( ull i = 0; i < shape.size; ++i )
				this->r_data[i] = r_data[i];
		}
	}
	template < uint N = 2 >
	Tensor( const uint ( &dimsSize )[N] = { 3, 3 }, const T *r_data = nullptr )
	    : Tensor( TensorShape( dimsSize, N ), r_data ) {}
	template < uint N = 1 >
	Tensor( const uint ( &dimsSize )[N], const T ( &r_data_list )[] )
	    : Tensor( TensorShape( dimsSize, N ), r_data_list ) {}
	template < uint N = 2, ull M >
	Tensor( const uint ( &dimsSize )[N], const T ( &r_data_list )[][M] )
	    : Tensor( TensorShape( dimsSize, N ), r_data_list[0] ) {}
	template < uint N = 3, ull M, ull M1 >
	Tensor( const uint ( &dimsSize )[N], const T ( &r_data_list )[][M1][M] )
	    : Tensor( TensorShape( dimsSize, N ), r_data_list[0][0] ) {}
	// to get view
	Tensor( TensorShape &&shape, T *r_data, ull size ) : shape( std::move( shape ) ), r_data( r_data ), size( size )
	{
		// this->shape.isnView = false;
	}
	// caution!! 暂时不要在view上使用，最好先copy
	void reshape( const TensorShape &shape )
	{
		this->shape = shape;
	}
	// caution!! 暂时不要在view上使用，最好先copy
	template < uint N = 2 >
	void reshape( const uint ( &dimsSize )[N] )
	{
		this->reshape( TensorShape( dimsSize, N ) );
	}
	// copy() not copy creater and grad
	Tensor( TensorShape &&shape ) : shape( std::move( shape ) ), size( shape.size ), r_data( new T[shape.size] ) {}
	Tensor( const Tensor &t ) : shape( t.shape ), size( t.size )  // creater->transer?
	{
		if ( t.shape.isnView )
		{
			this->r_data = new T[t.size];
			for ( ull i = 0; i < t.size; ++i )
				this->r_data[i] = T( t.r_data[i] );
		}
		else
			this->r_data = t.r_data;
	}
	Tensor( Tensor &&t ) : shape( std::move( t.shape ) ), r_data( t.r_data ), size( t.size )
	{
		t.r_data = nullptr;
	}

	// template < typename T2 >
	// operator Tensor< T2 >()
	// {  // todo
	// 	return Tensor< T2 >( this->shape, this->r_data );
	// }

	//[in test]
	Tensor &operator=( const Tensor &t )
	{
		if ( this != &t )
		{
			if ( this->shape.isnView && this->shape.isnSlice )
			{
				delete[] this->r_data;
				this->shape = t.shape;
				this->size = t.size;
				if ( t.shape.isnView )
				{
					this->r_data = new T[t.shape.size];
					for ( ull i = 0; i < t.shape.size; ++i )
						this->r_data[i] = t.r_data[i];
				}
				else
					this->r_data = t.r_data;
			}
			else
			{
				if ( this->shape != t.shape )
					throw std::runtime_error( "Tensor = &: replace a seq(view) of tnesor size not equal" );
				ull k, tk, index, tindex, *stride = this->shape.stride, *tstride = t.shape.stride, ksize;
				uint j, dim = this->shape.dim, *dimsSize = this->shape.dimsSize;
				for ( ull i = 0; i < t.shape.size; ++i )
				{
					ksize = this->shape.size;
					k = i;
					tk = i;
					// caution !!!
					index = this->shape.offset;
					tindex = t.shape.offset;
					for ( j = 0; j < dim; ++j )
					{
						ksize /= dimsSize[j];
						index += k / ksize * stride[j];
						k %= ksize;
						tindex += tk / ksize * tstride[j];
						tk %= ksize;
					}
					this->r_data[index] = t.r_data[tindex];
				}
			}
		}
		return *this;
	}
	//[in test]
	Tensor &operator=( Tensor &&t )
	{
		if ( this != &t )
		{
			if ( this->shape.isnView && this->shape.isnSlice )
			{
				delete[] this->r_data;
				this->shape = std::move( t.shape );
				this->r_data = t.r_data;
				this->size = t.size;
			}
			else
			{
				if ( this->shape != t.shape )
					throw std::runtime_error( "Tensor = &&: replace a seq(view) of tnesor size not equal" );
				ull k, tk, index, tindex, ksize;
				uint j, dim = this->shape.dim, *dimsSize = this->shape.dimsSize;
				ull *stride = this->shape.stride, *tstride = t.shape.stride;
				for ( ull i = 0; i < t.shape.size; ++i )
				{
					ksize = this->shape.size;
					k = i, tk = i;
					index = this->shape.offset;
					tindex = t.shape.offset;
					for ( j = 0; j < dim; ++j )
					{
						ksize /= dimsSize[j];
						index += k / ksize * stride[j];
						k %= ksize;
						tindex += tk / ksize * tstride[j];
						tk %= ksize;
					}
					this->r_data[index] = t.r_data[tindex];
				}
			}
			t.r_data = nullptr;
		}
		return *this;
	}
	// slice
	Tensor operator()( const uint start, uint end = 0, uint step = 1, uint oper_dim_idx = 0 )
	{
		if ( this->shape.dim <= oper_dim_idx )
			throw std::runtime_error( "Tensor: slice operDim out of Tensor,use oneValue to get" );
		if ( end >= this->shape.dimsSize[oper_dim_idx] )
			end = shape.dimsSize[oper_dim_idx] - 1;
		else if ( end == 0 )
		{
			end = start;
			step = 1;
		}
		else if ( end < start )
			throw std::runtime_error( "Tensor: slice end < start" );
		if ( step == 0 )
			throw std::runtime_error( "Tensor: slice step can't be 0" );

		// if ( this->shape.isnSlice )
		// {
		TensorShape newShape(
		    true,
		    this->shape.dim,
		    this->shape.size / this->shape.dimsSize[oper_dim_idx],
		    this->shape.offset + start * this->shape.stride[oper_dim_idx],
		    new uint[this->shape.dim],
		    new ull[this->shape.dim] );
		newShape.isnSlice = false;
		for ( uint i = 0; i < this->shape.dim; ++i )
		{
			newShape.dimsSize[i] = this->shape.dimsSize[i];
			newShape.stride[i] = this->shape.stride[i];
		}
		newShape.dimsSize[oper_dim_idx] = ( end - start ) / step + 1;
		newShape.size *= newShape.dimsSize[oper_dim_idx];
		newShape.stride[oper_dim_idx] *= step;
		return Tensor( std::move( newShape ), this->r_data, this->size );
		// }
		// this->shape.size /= this->shape.dimsSize[operDim];
		// this->shape.dimsSize[operDim] = ( end - start ) / step + 1;
		// this->shape.size *= this->shape.dimsSize[operDim];
		// this->shape.offset += start * this->shape.stride[operDim];
		// this->shape.stride[operDim] *= step;
		// return *this;
	}
	// 如果已经是视图，直接修改原shape [x](while exp2.Liner_Dis)
	// todo :如果是连续使用（右值）直接修改原shape
	// 上同
	Tensor operator[]( const uint i ) const
	{
		if ( this->shape.dim == 0 || i >= this->shape.dimsSize[0] )
			throw std::runtime_error( "Tensor: index out of range" );
		// if ( this->shape.isnView )
		// {
		TensorShape newShape(
		    false,
		    this->shape.dim - 1,
		    this->shape.size / this->shape.dimsSize[0],
		    this->shape.offset + i * this->shape.stride[0],
		    this->shape.dimsSize + 1,
		    this->shape.stride + 1 );
		return Tensor( std::move( newShape ), this->r_data, this->size );
		// }
		// this->shape.size /= this->shape.dimsSize[0];
		// this->shape.offset += i * this->shape.stride[0];
		// --this->shape.dim;
		// ++this->shape.dimsSize;
		// ++this->shape.stride;
		// return *this;
	}
	Tensor transpose( uint dim1_idx = 0, uint dim2_idx = 1 )
	{
		TensorShape newShape( this->shape );
		std::swap( newShape.stride[dim1_idx], newShape.stride[dim2_idx] );
		std::swap( newShape.dimsSize[dim1_idx], newShape.dimsSize[dim2_idx] );
		newShape.isnView = false;
		return Tensor( std::move( newShape ), this->r_data, this->size );
	}
	// only copy the r_data in shape

	Tensor copy() const
	{
		TensorShape newShape( this->shape.dimsSize, this->shape.dim );
		Tensor t( std::move( newShape ) );
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
			t.r_data[i] = this->r_data[index];
		}
		return t;
	}
	Tensor sum() const
	{
		T sum = 0;
		ull index, k, ksize;
		uint j;
		uint dim = this->shape.dim;
		ull *tstride = this->shape.stride;
		for ( ull i = 0; i < this->shape.size; ++i )
		{
			ksize = this->shape.size;
			k = i;
			index = this->shape.offset;
			for ( j = 0; j < dim; ++j )
			{
				ksize /= this->shape.dimsSize[j];
				index += k / ksize * tstride[j];
				k %= ksize;
			}
			sum += this->r_data[index];
		}
		return Tensor::OneValue( sum );
	}
	Tensor sum( uint dim_index ) const
	{
		uint tmp = this->shape.dimsSize[dim_index];
		this->shape.dimsSize[dim_index] = 1;
		TensorShape newShape( this->shape.dimsSize, this->shape.dim );
		this->shape.dimsSize[dim_index] = tmp;
		Tensor t( std::move( newShape ) );
		ull index, k;
		uint j;
		uint dim = this->shape.dim;
		ull *tstride = this->shape.stride;
		ull *stride = t.shape.stride;
		T sum;
		for ( ull i = 0; i < t.size; ++i )
		{
			k = i;
			index = this->shape.offset;
			for ( j = 0; j < dim; ++j )
			{
				index += k / stride[j] * tstride[j];
				k %= stride[j];
			}
			sum = 0;
			for ( k = 0; k < this->shape.dimsSize[dim_index]; ++k )
				sum += this->r_data[index + k * this->shape.stride[dim_index]];
			t.r_data[i] = sum;
		}
		return t;
	}
	Tensor max( uint dim_index ) const
	{
		uint tmp = this->shape.dimsSize[dim_index];
		this->shape.dimsSize[dim_index] = 1;
		TensorShape newShape( this->shape.dimsSize, this->shape.dim );
		this->shape.dimsSize[dim_index] = tmp;
		Tensor t( std::move( newShape ) );
		ull index, k;
		uint j;
		uint dim = this->shape.dim;
		ull *tstride = this->shape.stride;
		ull *stride = t.shape.stride;
		T sum;
		for ( ull i = 0; i < t.size; ++i )
		{
			k = i;
			index = this->shape.offset;
			for ( j = 0; j < dim; ++j )
			{
				index += k / stride[j] * tstride[j];
				k %= stride[j];
			}
			sum = -MAX< T >();
			for ( k = 0; k < this->shape.dimsSize[dim_index]; ++k )
				if ( sum < this->r_data[index + k * this->shape.stride[dim_index]] ) sum = this->r_data[index + k * this->shape.stride[dim_index]];
			t.r_data[i] = sum;
		}
		return t;
	}

	template < typename U >
	Tensor< U > to()
	{
		TensorShape newShape( this->shape.dimsSize, this->shape.dim );
		Tensor< U > t( std::move( newShape ) );
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
			t.r_data[i] = U( this->r_data[index] );
		}
		return t;
	}
	template < typename U >
	Tensor< U > *to_p()
	{
		TensorShape newShape( this->shape.dimsSize, this->shape.dim );
		Tensor< U > *t_p = new Tensor< U >( std::move( newShape ) );
		Tensor< U > &t = *t_p;
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
			t.r_data[i] = U( this->r_data[index] );
		}
		return t_p;
	}
	template < uint N >
	inline T &dataOper( const uint ( &a )[N] ) const
	{
		if ( ( this->shape.dim > 0 && N != this->shape.dim ) )
			throw std::runtime_error( "Tensor: dim not match" );
		if ( this->shape.dim == 0 )
			return this->r_data[this->shape.offset];
		uint index = shape.offset;
		for ( uint i = 0; i < N; ++i )
			index += a[i] * shape.stride[i];
		return r_data[index];
	}
	template < typename... Args >
	inline T &data( Args... args ) const
	{
		return dataOper( { static_cast< uint >( args )... } );
	}
	T &oneValue() const
	{
		// if ( this->shape.dim )
		// 	throw std::runtime_error( "Tensor: can't convert to scalar" );
		return this->r_data[this->shape.offset];
	}
	template < typename U, typename Caster >
	U &oneValue( Caster caster ) const
	{
		// if ( this->shape.dim )
		// throw std::runtime_error( "Tensor: can't convert to scalar" );
		return caster( this->r_data[this->shape.offset] );
	}
	inline static Tensor OneValue( const T &value )
	{
		Tensor t = Tensor( TensorShape( nullptr, 0 ), nullptr );
		t.r_data[0] = value;
		return t;
	}



	~Tensor()
	{
		if ( this->shape.isnView && this->shape.isnSlice )
			delete[] this->r_data;
	}
	// private:
	std ::ostream &printView( std ::ostream &os ) const
	{
		if ( this->shape.dim == 0 )
			return os << this->r_data[this->shape.offset] << '\n';
		if ( this->shape.dim == 1 )
		{
			os << '[' << this->r_data[this->shape.offset];
			for ( uint i = 1; i < this->shape.dimsSize[0]; ++i )
				os << ',' << this->r_data[this->shape.offset + i * this->shape.stride[0]];
			os << "]\n";
			return os;
		}
		os << "[\n";
		for ( uint i = 0; i < this->shape.dimsSize[0]; ++i )
			this->operator[]( i ).printView( os );
		os << "]\n";
		return os;
	}

	friend std::ostream &operator<<( std::ostream &os, const Tensor &t )
	{
		return t.printView( os );
	}

	// friend std::ostream &operator<<( std::ostream &os, Tensor &&t )
	// {
	// 	return t.printView( os );
	// }

	// static Tensor Mul( const Tensor &, const Tensor &, const uint (&)[], const uint (&)[] );

	Tensor operator+( const Tensor &t ) const;
	Tensor operator-() const;
	Tensor operator-( const Tensor &t ) const;
	Tensor operator*( const Tensor &t ) const;
	// 内积
	T operator^( const Tensor &t ) const;
	Tensor eMul( const Tensor &t ) const;
	Tensor nPow( const uint & ) const;

	Tensor operator/( const Tensor &t ) const;
	Tensor operator+( const T &t ) const;
	Tensor operator-( const T &t ) const;
	Tensor operator*( const T &t ) const;
	Tensor operator/( const T &t ) const;
	Tensor operator>( const T &t ) const;
	// Tensor operator<( const T &t ) const;


	Tensor &operator+=( const Tensor &t );
	Tensor &operator-=( const Tensor &t );
	Tensor &operator*=( const Tensor &t );
	Tensor &operator/=( const Tensor &t );
	Tensor &operator+=( const T &t );
	Tensor &operator-=( const T &t );
	Tensor &operator*=( const T &t );
	Tensor &operator/=( const T &t );
	static void LU_of( const Tensor &, Tensor< T > &, Tensor< T > &, const Tensor< uint > & );
	static Tensor Solve_LU( const Tensor &, const Tensor &, const Tensor< T > &, const Tensor< uint > & );
	static Tensor< uint > Max_I_I_Rearrange( const Tensor &, Tensor * = nullptr );
	static Tensor Identity( const uint n );

	inline static Tensor *All_of_p( const TensorShape &shape, T value )
	{
		Tensor *t = new Tensor( shape, nullptr );
		for ( ull i = 0; i < shape.size; ++i )
			t->r_data[i] = value;
		return t;
	}
	inline static Tensor All_of( const TensorShape &shape, T value )
	{
		Tensor t( shape, nullptr );
		for ( ull i = 0; i < shape.size; ++i )
			t.r_data[i] = value;
		return t;
	}
	Tensor oneHot( const uint &size ) const;
	Tensor< uint > index_of_max( uint dim_index ) const;
	Tensor< ull > equal( const Tensor &t ) const;
	static Tensor< T > Exp( const Tensor & );
	static Tensor< T > Ln( const Tensor & );

	Tensor< T > sigmoid() const;
	Tensor< T > ReLU() const;


	static Tensor FromCSV( const std::string &filename, bool hasHeader = false, char delimiter = ',', char endline = '\n' );
	bool toCSV( const std::string &filename, char delimiter = ',', char endline = '\n' ) const;
};

#include "tensor_oper.cpp"
#include "tensor_util.cpp"

#define t_data( a... ) dataOper( { a } )

template <>
template < typename U >
Tensor< U > Tensor< std::string >::to()
{
	TensorShape newShape( this->shape.dimsSize, this->shape.dim );
	Tensor< U > t( std::move( newShape ) );
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
		t.r_data[i] = sto< U >( this->r_data[index] );
	}
	return t;
}
template <>
template < typename U >
Tensor< U > *Tensor< std::string >::to_p()
{
	TensorShape newShape( this->shape.dimsSize, this->shape.dim );
	Tensor< U > *t_p = new Tensor< U >( std::move( newShape ) );
	Tensor< U > &t = *t_p;
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
		t.r_data[i] = sto< U >( this->r_data[index] );
	}
	return t_p;
}
// int test( int argc, char const *argv[] )
// {
// 	// TensorShape< 2 > shape( { 1, 2 } );
// 	Tensor< int > t( { 3, 4 } );
// 	int a = 0;
// 	for ( uint j = 0; j < 3; ++j )
// 		for ( uint k = 0; k < 4; ++k )
// 			t[j][k].oneValue() = ++a;
// 	// std::cout << t.shape << std::endl;

// 	for ( uint j = 0; j < 3; ++j )
// 	{
// 		for ( uint k = 0; k < 4; ++k )
// 			std::cout << t[j][k].oneValue() << " ";
// 		std::cout << std::endl;
// 	}
// 	auto tp = t.transpose().copy();
// 	auto &t1 = *tp;
// 	std::cout << std::endl
// 	          << t.shape
// 	          << std::endl
// 	          << t1.shape << std::endl;

// 	for ( uint j = 0; j < 4; ++j )
// 	{
// 		for ( uint k = 0; k < 3; ++k )
// 			std::cout << t1 /*( j, j, 1, 1 )( k, k, 1, 2 )*/.data( j, k ) << " ";
// 		std::cout << std::endl;
// 	}


// for ( uint j = 0; j < 4; ++j )
// {
// 	for ( uint k = 0; k < 3; ++k )
// 		std::cout << t[j][k].oneValue() << " ";
// 	std::cout << std::endl;
// }
// return 0;
// }
