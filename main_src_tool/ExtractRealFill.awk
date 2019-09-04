{
    if( NF == 8 )
    {
        c1=$1;
        c2=0+c1+1;
        len=split($8,info,"=");
        if( len == 2 )
        {
            printf(">%d_%d\n%s\n",c1,c2,info[2]);
        }
    }
    if( NF == 9 )
    {
        c1=$1;
        c2=0+c1+1;
        len=split($9,info,"=");
        if( len == 2 )
        {
            printf(">%d_%d\n%s\n",c1,c2,info[2]);
        }
    }
}
