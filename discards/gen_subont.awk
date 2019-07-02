BEGIN{
    id=1;
}
{
    if($12 != "N")
    {
        gap_id=0+$1;
        printf(">seq_id_%d_%d\n%s\n",gap_id,id,$12);
        id+=1;
    }
}

