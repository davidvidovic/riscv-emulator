int main()
{
	int a,b,c;
	int i;
	int arr[5];
	b = 1;
	c = 2;
	a = b + 5 + c + 11;
	arr[a+2] = 999;
	arr[1] = 888;

	for(i = 0; i < 5; i++)
	{
		arr[i] = 1;
	}


	return 0;
}