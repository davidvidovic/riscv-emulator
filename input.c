int foo(int par1)
{
	int a;
	a = 1;
	return par1+a; 
}

int main()
{
	int a,b,c,d,e,f,g, h, j, k, l, i;
	a = 1;
	b = 2;
	c = 3;
	d = 4;
	e = 5;
	f = 6;
	g = 7;
	h = 8;
	j = 9; 
	k = 10;
	l = 11;

	for(i = 0; i <= 10; ++i)
	{
		k += i + foo(i);
	}	
	
	j = 88;
	f = 777;

	switch(f)
	{
		case 0:
			a = 0;
		break;

		case 777:
			h = 1;
		break;

		default:
			a = 2;
		break;
	}
	
	if(a < 11)
	{
		d = 444;
	}
	else	
	{
		d = 555;
	}
	c = 11;


	return 0;
}


