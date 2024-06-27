int foo(int g, int par1, int par2)
{
	int b, a = 19;
	b = a + par1 - g;
	return b;
}

int foo1(int a)
{
	a = a + 1919;
	return a;
}

int main()
{
	int a,b,c;
	int i;
	int arr[5];
	b = 2;
	a = 99;

	foo(a, b, 19);
	foo1(b);

	a = 9;
	return 0;
}