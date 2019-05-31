#!/bin/bash

if [[ $# != 2 ]] ; then 
    echo "usage : $0 gap_id input.txt "
    exit 1 ;
fi

target=$1
echo "Target gap id $target"
echo "BEGIN{" >tmp.awk
echo "  id=1;"  >>tmp.awk
echo "}"      >>tmp.awk
echo "{"      >>tmp.awk
echo "  if(\$1==$target && \$12 != \"N\")" >>tmp.awk
echo "  {"    >>tmp.awk
echo "      printf(\">seq_id_%d\n%s\n\",id,\$12);" >>tmp.awk
echo "      id+=1;" >>tmp.awk
echo "  }">>tmp.awk
echo "}" >>tmp.awk


awk -f tmp.awk <$2 >ont.tmp.$target.fa
