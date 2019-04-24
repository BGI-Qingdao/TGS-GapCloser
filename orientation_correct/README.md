# brief

    ONT for scaff_infos orientation correct pipeline 
    from wangwenchao@genomics.cn

# pipeline usage 

    minmap/type_change/oc/filter_oo为工具，只需要修改最后"/"之前的路径即可
    minimap 是将contig比对到ont上
    type_change工具则是将minimap产生的paf文件转化为两个contig之间四个方向（FF,FR,RF,RR）上被ont验证的条数的关系
    oc对应的工具是使用上面的结果纠正orientation

    需要注意的是：
    awk对应的是筛选掉比对质量为0的情况
    filter_oo是要求两个contig之间的方向（验证条数最大的）上验证的ont条数要>=3
    这两个条件可以根据实际的情况选择性使用。

