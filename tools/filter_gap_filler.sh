#!/bin/bash

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
