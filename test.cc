#include <cstdio>

int main()
{
	int a = 5, b = 10;
	a = a + b - ( b = a );
	printf( "%d %d\n", a, b );
	scanf( "%d", &a );
}