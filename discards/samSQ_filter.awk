BEGIN{
    t["none"]=1;
}
{
    if( $1 == "@SQ" )
    {
        if( $2 in t )
        {
            ;
        }
        else
        {
            t[$2]=1;
            print $0;
        }
    }
    else
    {
        print $0;
    }

}
