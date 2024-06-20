int main()
{
	int a,b;
	a = 3;
	b = 0;

	for(a = 0; a < 10; ++a)
	{
		b = a + 2;
		if(b > 5)
		{
			continue;
		}
		b = b + b;
	}
	b = 10000;
	

	switch(a)
	{
		case 1:
			b = 1111;
			break;
		
		default:
			b = 2222;
			break;
		
		case 2:
			b = 3333;
			break;

		case 3:
			b = 4444;
			break;

		
	}
	a = 2;
}