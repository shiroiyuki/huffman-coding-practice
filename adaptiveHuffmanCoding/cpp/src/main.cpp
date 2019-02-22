#include <cmath>
#include "ahc.hpp"

#define MAX 256
#define FILESIZE 262144
#define ProbNum FILESIZE-1
void print_help()
{
	puts("ahc [-e|-x|-d|-r infile outfile][-f infile]");
	puts("\t -h print help");
	puts("\t -e encode");
	puts("\t -x decode");
	puts("\t -d dpcm");
	puts("\t -r recover");
	puts("\t -f first order entropy");
    return;
}

long double entropy(long double * src){
    long double num = 0; 
    for(int i =0;i<MAX;i++){
        if(src[i]==0)
            continue;
        else
            num += src[i]*log2(src[i]);
    }
    return -num;
}

long double first_order_markov(long double * src1,long double src2[][MAX]){
    long double num = 0;
    for(int y =0;y<MAX;y++){
        for(int x =0;x<MAX;x++){
            if(src1[y]==0 || src2[x][y]==0)
                continue;
            else
                num += src1[y]*src2[x][y]*log2(src2[x][y]);
        }
    }
    return -num;
}

void foe(const char* filename){
    long double count[MAX] ={0};
    long double order[MAX][MAX] ={0};
    FILE * input = nullptr;
    int16_t c;
    int16_t b;
    input = fopen(filename,"r+");
    b = fgetc(input);
    count[b]++;
    while((c = fgetc(input)) != EOF){
        count[c]++;
        order[c][b]++;
        b = c;
    }
    for(int i =0;i<MAX;i++)
        count[i] /=FILESIZE;
    for(int y =0;y<MAX;y++)
        for(int x =0;x<MAX;x++){
            order[y][x] /=ProbNum;
        }
    long double num = 0;
    for(int i =0;i<MAX;i++)
        for(int j =0;j<MAX;j++)
            num +=order[i][j];
    printf("Entropy: %Lf bit/symbol\n",entropy(count));
    //printf("First Order Markov Model: %Lf\n",first_order_markov(count,order));
    fclose(input);
    return;
}

void dpcm(int Width,int Height, unsigned char* yBuf,unsigned char* devBuf,unsigned char* rebuild_Buf){
    unsigned char pred=128;//假设第一个像素的预测值为128
    for(int i=0;i<Height;i++){
        for(int j=0;j<Width;j++){
            devBuf[i*Width+j] = (yBuf[i*Width+j]-pred)/2+127;
            rebuild_Buf[i*Width+j]=(devBuf[i*Width+j]-127)*2 + pred;// 重建值
            pred = rebuild_Buf[i*Width+j];
        }
        pred = rebuild_Buf[i*Width];
    }
	return;
}

void dpcmRecover(int Width,int Height, unsigned char* devBuf,unsigned char* rebuild_Buf){
    unsigned char pred=128;//假设第一个像素的预测值为128
    for(int i=0;i<Height;i++){
        for(int j=0;j<Width;j++){
            rebuild_Buf[i*Width+j]=(devBuf[i*Width+j]-127)*2 + pred;// 重建值
            pred = rebuild_Buf[i*Width+j];
        }
        pred = rebuild_Buf[i*Width];
    }
	return;
}

void preprocess(const char* infile, const char* outfile){
    unsigned char src[FILESIZE] ={0};
    unsigned char dev[FILESIZE] ={0};
    unsigned char reb[FILESIZE] ={0};
    FILE * input = nullptr;
    FILE * output = nullptr;
    int index = 0;
    int16_t c;
    input = fopen(infile,"r+");
    output = fopen(outfile,"wb+");
    while((c = fgetc(input)) != EOF)
        src[index++] = c;
    dpcm(512,512,src,dev,reb);
    for(int i =0; i<FILESIZE;i++)
        fprintf(output,"%c",dev[i]);
    fclose(input);
    fclose(output);
}

void postprocess(const char* infile, const char* outfile){
    unsigned char dev[FILESIZE] ={0};
    unsigned char reb[FILESIZE] ={0};
    FILE * input = nullptr;
    FILE * output = nullptr;
    int index = 0;
    int16_t c;
    input = fopen(infile,"r+");
    output = fopen(outfile,"wb+");
    while((c = fgetc(input)) != EOF)
        dev[index++] = c;
    dpcmRecover(512,512,dev,reb);
    for(int i =0; i<FILESIZE;i++)
        fprintf(output,"%c",reb[i]);
    fclose(input);
    fclose(output);
}

int main(int argc, char **argv){
    if(argc>1){
        if(!strcmp(argv[1],"-h")){
            print_help();
            return 0;
        }
        if(!strcmp(argv[1],"-e")){
            AHEncoder enc;
            enc.setInputFile(argv[2]);
            enc.codeBuffer.setOutputFile(argv[3]);
            enc.encoding();
        }
        if(!strcmp(argv[1],"-x")){
            AHDecoder dec;
            dec.setInputFile(argv[2]);
            dec.codeBuffer.setOutputFile(argv[3]);
            dec.decoding();
        }
        if(!strcmp(argv[1],"-d")){
            preprocess(argv[2],argv[3]);
        }
        if(!strcmp(argv[1],"-r")){
            postprocess(argv[2],argv[3]);
        }
        if(!strcmp(argv[1],"-f")){
            foe(argv[2]);
        }
    }
    else{
        print_help();
    }
    return 0;
}