#include "arr.cc"
#include <cstdio>
int main()
{
	int a = 5, b = 10;
	a = a + b - ( b = a );
	Arr< int > arr( 10 );
	printf( "%d %d\n", a, b );
	scanf( "%d", &a );
}