// 主要用于重载操作符
// 这里面的直接重置tensor操作都是高危操作（不支持切片）
#pragma once
#include "../oper/oper.hpp"
#include "tensor.cc"
#ifndef TENSOR_GRAD_TYPE
	#define TENSOR_GRAD_TYPE double
#endif
template < typename T >
struct TensorHolder
{
	Tensor< T > tensor;
	TensorHolder< TENSOR_GRAD_TYPE > *gradHolder = nullptr;
private:
	bool needGrad = false;
public:
	const bool hasGrad;
	bool gradCleared = true;
	Oper< T > *creator = nullptr;
	// 若初始设置no needGrad 就最好别来沾边
	TensorHolder( const Tensor< T > &tensor, bool needGrad = true, Oper< T > *creator = nullptr )
	    : tensor( tensor ), needGrad( needGrad ), creator( creator ), hasGrad( needGrad )
	{
		if ( needGrad )
		// 默认只支持1阶梯度
		{
			this->gradHolder = new TensorHolder< TENSOR_GRAD_TYPE >( Tensor< TENSOR_GRAD_TYPE >::All_of( tensor.shape, 0 ), false );
			this->gradCleared = true;
		}
	}
	// TensorHolder( const TensorShape &TensorShape, bool needGrad = false, Oper< T > *creator = nullptr )
	//     : tensor( TensorShape ), needGrad( needGrad ), creator( creator ), hasGrad( needGrad )
	// {
	// }
	TensorHolder( const TensorHolder< T > &other ) : tensor( other.tensor ), needGrad( other.needGrad ), creator( other.creator ), gradCleared( other.gradCleared ), hasGrad( other.hasGrad )
	{
		if ( other.hasGrad )
			this->gradHolder = new TensorHolder< TENSOR_GRAD_TYPE >( *other.gradHolder );
	}
	TensorHolder( TensorHolder< T > &&other ) : tensor( other.tensor ), needGrad( other.needGrad ), gradHolder( other.gradHolder ), creator( other.creator ), gradCleared( other.gradCleared ), hasGrad( other.hasGrad )
	{
		other.gradHolder = nullptr;
		other.creator = nullptr;
	}
	~TensorHolder()
	{
		delete this->gradHolder;
		delete this->creator;
	}
	TensorHolder &operator=( const TensorHolder &other )
	{
		this->tensor = other.tensor;
		delete this->gradHolder;
		delete this->creator;
		this->needGrad = other.needGrad;
		this->creator = other.creator;
		if ( this->hasGrad && other.hasGrad && !( this->gradCleared && other.gradCleared ) )
			this->gradHolder = new TensorHolder< TENSOR_GRAD_TYPE >( *other.gradHolder );
		return *this;
	}
	TensorHolder &operator=( TensorHolder &&other )
	{
		this->tensor = other.tensor;
		delete this->gradHolder;
		delete this->creator;
		this->needGrad = other.needGrad;
		this->creator = other.creator;
		this->gradHolder = other.gradHolder;
		other.gradHolder = nullptr;
		other.creator = nullptr;
		return *this;
	}
	void setNeedGrad( bool needGrad )
	{
		if ( !this->hasGrad )
			throw std::runtime_error( "TensorHolder: can't set needGrad" );
		this->needGrad = needGrad;
	}
	void set( const Tensor< T > &tensor )
	{
		this->tensor = tensor;
		// if ( this->needGrad && !this->gradCleared )
		// 	this->clearGrad();
	}
	// dangerous!
	template < ull N >
	void set( const T ( &data )[N] )
	{
		for ( ull i = 0; i < N && i < this->tensor.size; ++i )
			this->tensor.r_data[i] = data[i];
	}
	TensorHolder< TENSOR_GRAD_TYPE > &grad()
	{
		return *this->gradHolder;
	}
	void cal()
	{
		if ( this->creator )
			this->creator->exec( *this );
	}

	bool clearGrad()
	{
		if ( this->gradCleared )
			return true;
		if ( this->creator ) this->creator->clearGrad();
		if ( this->needGrad )
		{
			for ( ull i = 0; i < this->tensor.size; ++i )
				this->gradHolder->tensor.r_data[i] = 0;
			// 清空构建
			delete this->gradHolder->creator;
			this->gradHolder->creator = nullptr;
			this->gradHolder->clearGrad();
		}
		this->gradCleared = true;
		return true;
	}

	bool buildGradAsMain()
	{
		if ( this->tensor.shape.size != 1 )
			throw std::runtime_error( "TensorHolder: can't backward/has not implemented yet" );
		if ( !this->hasGrad )
			throw std::runtime_error( "TensorHolder: must has gradHolder" );
		if ( !this->needGrad )
			return false;
		for ( ull i = 0; i < this->tensor.size; ++i )
			this->gradHolder->tensor.r_data[i] = 1;
		if ( this->creator )
		{
			// 必要吗
			// 必要，因为grad+= 把东西拷贝出去一份到非自然成图
			// 而且本来一般build后也会clear
			this->creator->clearGrad();
			this->creator->buildGrad( *this );
			this->gradCleared = false;
			return true;
		}
		return false;
	}
	// bool calGrad( )
	// {
	// 	if ( !this->needGrad )
	// 		return false;
	// 	this->gradCleared = false;
	// 	this->creator->calGrad( *this );
	// 	return true;
	// }
	friend std::ostream &operator<<( std::ostream &os, TensorHolder &tensorHolder )
	{
		os << tensorHolder.tensor;
		return os;
	}
	// TensorHolder operator+( TensorHolder &other )
	// 	{
	// 		if ( this->tensor.shape != other.tensor.shape )
	// 			throw std::runtime_error( "TensorHolder +: shape not match" );
	// 		Add< T > *op = new Add< T >( this, &other );
	// 		return TensorHolder( Tensor< T >( this->tensor.shape ), this->hasGrad || other.hasGrad, op );
	// 	}
	TensorHolder &operator+=( TensorHolder &other )
	{
		// 相信我的广播机制!
		//  if ( this->tensor.shape == other.tensor.shape )
		//  {
		Add< T > *op = new Add< T >( new TensorHolder( *this ), &other, true );
		this->creator = op;  // 0xeb4de0
		return *this;
		// }
		// throw std::runtime_error( "TensorHolder +=: shape not match" );
	}
	TensorHolder &operator-=( TensorHolder &other )
	{
		// if ( this->tensor.shape == other.tensor.shape )
		// {
		Sub< T > *op = new Sub< T >( new TensorHolder( *this ), &other, true );
		this->creator = op;  // 0xeb4de0
		return *this;
		// }
		// throw std::runtime_error( "TensorHolder -=: shape not match" );
	}
	// 普通mul,指定二维
	static TensorHolder Mul2D( TensorHolder &a, TensorHolder &b, uint a_raw_dim, uint a_col_dim, uint b_raw_dim, uint b_col_dim )
	{
		// todo
	}
};
template < typename T >
TensorHolder< T > operator+( TensorHolder< T > &th, TensorHolder< T > &other )
{
	// if ( th.tensor.shape == other.tensor.shape )
	// {
	Add< T > *op = new Add< T >( &th, &other );
	return TensorHolder( Tensor< T >( th.tensor.shape ), th.hasGrad || other.hasGrad, op );
	// }
	// throw std::runtime_error( "TensorHolder +: shape not match" );
}
template < typename T >
TensorHolder< T > operator+( TensorHolder< T > &&th, TensorHolder< T > &other )
{
	// if ( th.tensor.shape == other.tensor.shape )
	// {
	Add< T > *op = new Add< T >( new TensorHolder( std::move( th ) ), &other, true );
	return TensorHolder( Tensor< T >( th.tensor.shape ), th.hasGrad || other.hasGrad, op );
	// }
	// throw std::runtime_error( "TensorHolder +: shape not match" );
}
template < typename T >
TensorHolder< T > operator-( TensorHolder< T > &th, TensorHolder< T > &other )
{
	// if ( th.tensor.shape == other.tensor.shape )
	// {
	Sub< T > *op = new Sub< T >( &th, &other );
	return TensorHolder( Tensor< T >( th.tensor.shape ), th.hasGrad || other.hasGrad, op );
	// }
	// throw std::runtime_error( "TensorHolder -: shape not match" );
}
template < typename T >
TensorHolder< T > operator-( TensorHolder< T > &&th, TensorHolder< T > &other )
{
	// if ( th.tensor.shape == other.tensor.shape )
	// {
	Sub< T > *op = new Sub< T >( new TensorHolder( std::move( th ) ), &other, true );
	return TensorHolder( Tensor< T >( th.tensor.shape ), th.hasGrad || other.hasGrad, op );
	// }
	// throw std::runtime_error( "TensorHolder -: shape not match" );
}


template < typename T >
TensorHolder< T > operator*( TensorHolder< T > &th, TensorHolder< T > &other )
{
	if ( th.tensor.shape == other.tensor.shape )
	{
		Mul< T > *op = new Mul< T >( &th, &other );
		return TensorHolder( Tensor< T >( th.tensor.shape ), th.hasGrad || other.hasGrad, op );
	}
	throw std::runtime_error( "TensorHolder +: shape not match" );
}
template < typename T >
TensorHolder< T > operator*( TensorHolder< T > &&th, TensorHolder< T > &other )
{
	if ( th.tensor.shape == other.tensor.shape )
	{
		Add< T > *op = new Add< T >( new TensorHolder( std::move( th ) ), &other, true );
		return TensorHolder( Tensor< T >( th.tensor.shape ), th.hasGrad || other.hasGrad, op );
	}
	throw std::runtime_error( "TensorHolder +: shape not match" );
}