int main()
{ 
    int a = 1;
    int b = 2, c;

    if(a != b)
    {
        b = b + 19;
        while(b > 15)
        {
            b = b - 2;
        }
        c = 0;
    }
    else
    {
        for(a = 1; a < 4; a=a+1)
        {
            b = b + a;
        }
    }
    c = 1;
}