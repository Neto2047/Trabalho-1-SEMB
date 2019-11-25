#include<stdio.h>
#include<stdlib.h>

unsigned char getBit(unsigned char *v, unsigned char i, unsigned char j){
  unsigned short m=0,n=0;
  unsigned char mask = 1;
  m = (i*160+j)/8;
  n = (i*160+j)%8;
  mask = mask << n;
  return ((v[m] & mask) >> n);
}

int main(){
  unsigned char img[2400];
  FILE *frd
  FILE *fwr;
  unsigned char c;
  frd = fopen("arquivo.txt","r"); //arquivo contendo a saida
  fwr = fopen("img.pgm","w");	  //arquivo onde a imagem sera gravada
  for(int i=0;i<2400;++i){
      img[i] = fgetc(frd);
  }
  fclose(frd);
  fprintf(fwr,"P5\n160 120\n255\n");
  for(int i=0;i<120;++i){
    for(int j=0;j<160;++j){
      if(getBit(img,i,j) == 1){
        fprintf(fwr,"%c",255);
      }else{
	fprintf(fwr,"%c",0);
      }
    }
  }
  return 0;
}
