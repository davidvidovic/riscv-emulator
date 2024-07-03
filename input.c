int foo(int par1)
{
	return ++par1;
}

int main()
{
	int a, b, c, i;
	int arr[5];
	b = 1;
	a = 4097;
	a = a + b;
	c = 19 + b - a;
	for(i = 0; i < 10; i=i+1)
	{
		arr[i] = c + i;
		a = a + 888;
	}
	
	b = foo(b);

	if(b < 19)
	{
		a = 999;
	}
	else
	{
		c = 0;
	}

	i = 0;
	
	return 0;
}
