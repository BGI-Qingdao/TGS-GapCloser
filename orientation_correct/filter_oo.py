#!/usr/bin/python
# from wangwenchao@genomics.cn
# /nascngb/gccnt/wangou/assemblerdev/wangwenchao/sz_test/scripts/oo_pipeline.sh
from optparse import OptionParser
import re
parser = OptionParser()
parser.add_option("--in1", dest="infile1", help="give a oo_file", metavar="FILE")
#parser.add_option("--in2", dest="infile2", help="give a scaff2contig file to me", metavar="FILE")
(options, args) = parser.parse_args()
f = file(options.infile1,"r")
lis=[]
for line in f:
    binfo=re.split('\t',line.strip())
    c1=binfo[0]
    c2=binfo[1]
    if c1==c2:
        continue
    else:
        v=max(map(int,binfo[2:]))
        if v<3:
            continue
        else:
            #s1=get_key(dic1,c1)
            #s2=get_key(dic1,c2)
            #print s1,s2
            #if map(int,binfo[2:]).count(v)==1 and s1!=[] and s2!=[] and s1==s2:
            lis.append(line.strip())
for j in lis:
    print j

