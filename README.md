# TGS Gap filler

A gap filling tool that use thild generation sequences reads to fill genome.

- Both raw-reads and reads-after-error-correction can be used as input .

- If you have NGS reads , it can polish the raw TGS reads by called polion .

## How to install

### downloading 
```
git clone  https://github.com/BGI-Qingdao/TGSGapFiller.git YOUR-INSTALL-DIR
```

### compileing

#### prepare minimap2

- If you already have a minimap2 , you can link it here

```
ln -s MINIMAP2-PATH YOUR-INSTALL-DIR/minimap2
```
- Otherwise you can download it by :

```
cd YOUR-INSTALL-DIR
git submodule init
git submodule update
```

#### compileing

```
cd YOUR-INSTALL-DIR/main_src_ont
make
```

## Usage 


### without polish version

- cp YOUR-INSTALL-DIR/pipeline/pipeline.sh into your-working-directory
- edit pipeline.sh for your project
- run pipeline.sh

### with polish version

- cp YOUR-INSTALL-DIR/pipeline/pipeline_polish.sh into your-working-directory
- edit pipeline_polish.sh for your project
- run pipeline.sh

## Output

- your-prefix.scaff_seq 
    - this is the final sequence after gap filling
- your-prefix.gap_fill_details
    - details about how the final sequence was assemblied 

### format of your-prefix.gap_fill_details

#### a example :

```
>scaffold_1
1	1000	S	1000	2000
1001	1010	N
1011	1100	S	2201	2290
1101	1110	F
1111	1200	S	2301	2390
>scaffold_2
......

```
#### details information

1. each scaffold name followed by it's own data lines.
2. data line contains 3 or 5 columns and describe the source of a segment sequence of the final sequence:
    - column 1 is the segment first position at the final sequence.
    - column 2 is the segment last position at the final sequence.
    - column 3 is the segment type , which can only be 'S' , 'N' or 'F'.
        - if 'S' , it means this segment is a segment of input sequence and this line will have another 2 column :
            - column 4 is the segment first position at the input sequence.
            - column 5 is the segment last position at the input sequence.
        - if 'N' , it means this segment is N area.
        - if 'F' , it means this segment is a filled sequence from TGS reads .

