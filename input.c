int foo(int g, int par1, int par2)
{
	int b, a = 19;
	b = a + par1 - g;
	return b;
}

int foo1(int a)
{
	a = a + 100;
	if(a > 1000)
	{
		return a;
	}

	foo1(a);
}

int main()
{
	int a,b,c;
	int i;
	int arr[5];
	b = 2;
	a = 99;

	c = foo(a, b, 19);
	b = 199 - foo1(a);

	a = 9;
	return 0;
}