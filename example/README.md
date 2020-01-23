
# example case

## Prepare

Make sure you have already built this software, if not, try

```
cd ../ && make 
```

## Run  

### run with racon 

```
../TGS-GapCloser.sh --scaff input.scaff.fasta --reads ont.reads.fasta --output test_racon --racon YOUR-RACON-FILE
```

### run with pilon

```
../TGS-GapCloser.sh --scaff input.scaff.fasta --reads ont.reads.fasta --output test_pilon --ngs ngs.reads.fastq  \
	--pilon  pilon-path/pilon-1.23.jar  \
        --samtools samtools-path/bin/samtools  \
        --java    java-path/bin/java \
```

### run without error-correction

```
../TGS-GapCloser.sh --scaff input.scaff.fasta --reads ont.reads.fasta --output test_ne --ne
```
