#include "tensor.hpp"
int main()
{
	Tensor< double > at( { 2, 3 }, { 1, 2, 3,
	                                 4, 5, 6 } );
	Tensor< double > bt( { 3, 2 }, { 1, 2,
	                                 3, 4,
	                                 5, 6 } );
	// TensorHolder< double > a( at );
	// TensorHolder< double > b( bt );
	// auto d = a * b;
	// auto c = d.sum();
	// c.force_cal();
	// std::cout << c << d << std::endl;

	// c.buildGradAsMain();
	// a.grad().cal();
	// std::cout << a.grad() << std::endl;
	// at -= a.grad().tensor;
	// c.force_cal();
	// std::cout << c << std::endl;
}