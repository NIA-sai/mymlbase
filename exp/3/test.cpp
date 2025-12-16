#include "tensor.hpp"

int main()
{
	TensorHolder< double > a( { { 1 } } ), b( { { 1 } } );
	TensorHolder< double > c = (a + b) + a;

	a.set( { 1 } );
	b.set( { 2 } );

	c.buildGradAsMain();
	a.grad().cal();
	cout << a.grad() << endl;
}