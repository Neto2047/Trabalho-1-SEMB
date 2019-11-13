#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
int main(){
  //int c;
  FILE *f2rd, *f2wr;
  f2wr = fopen("code.c","a");
  f2rd = fopen("res.pgm","r");
  int i=0, j;

  for(i=0;i<15;i++){
    fgetc(f2rd);
  }
  fprintf(f2wr,"unsigned char myImg[120][160] = {");
  /*while((c = fgetc(f2rd))!=EOF){
    fprintf(f2wr,"%d,",(unsigned int)c);
  }*/
  for(i=0;i<19200;++i){
    fprintf(f2wr,"%d,",(unsigned int)fgetc(f2rd));
    if((i+1)%20 == 0){
      fprintf(f2wr,"\n");
    }
  }
  fprintf(f2wr,"};\n");
  fclose(f2rd);
  f2rd = fopen("main.c","r");
  int c = fgetc(f2rd);
  while(c != EOF){
    fputc(c,f2wr);
    c = fgetc(f2rd);
 }

  fclose(f2rd);
  fclose(f2wr);
  return 0;
}
