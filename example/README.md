
# example case

## Prepare

Make sure your already build this software , if not , try

```
cd ../ && make 
```

## Run  

### run with racon 

```
../TGSGapFiller.sh --scaff input.scaff.fasta --reads ont.reads.fasta --output test_racon --racon YOUR-RACON-FILE
```

### run with pilon

```
../TGSGapFiller.sh --scaff input.scaff.fasta --reads ont.reads.fasta --output test_pilon --ngs ngs.reads.fastq  \
	--pilon  pilon-path/pilon-1.23.jar  \
        --samtool samtools-path/bin/samtools  \
        --java    java-path/bin/java \
```

### run without error-correction

```
../TGSGapFiller.sh --scaff input.scaff.fasta --reads ont.reads.fasta --output test_ne --ne
```
