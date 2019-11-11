/*
 * File:   main.c
 * Author: elias
 *
 * Created on May 20, 2019, 3:46 PM
 */

// Production -> Set Configuration Bits;
// PIC18F47K42 Configuration Bit Settings
// 'C' source line config statements

// CONFIG1L
#pragma config FEXTOSC = OFF    // External Oscillator Selection (Oscillator not enabled)
#pragma config RSTOSC = HFINTOSC_64MHZ// Reset Oscillator Selection (HFINTOSC with HFFRQ = 64 MHz and CDIV = 1:1)

// CONFIG1H
#pragma config CLKOUTEN = OFF   // Clock out Enable bit (CLKOUT function is disabled)
#pragma config PR1WAY = ON      // PRLOCKED One-Way Set Enable bit (PRLOCK bit can be cleared and set only once)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)

// CONFIG2L
#pragma config MCLRE = EXTMCLR  // MCLR Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RE3 pin function is MCLR )
#pragma config PWRTS = PWRT_OFF // Power-up timer selection bits (PWRT is disabled)
#pragma config MVECEN = ON      // Multi-vector enable bit (Multi-vector enabled, Vector table used for interrupts)
#pragma config IVT1WAY = ON     // IVTLOCK bit One-way set enable bit (IVTLOCK bit can be cleared and set only once)
#pragma config LPBOREN = OFF    // Low Power BOR Enable bit (ULPBOR disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled , SBOREN bit is ignored)

// CONFIG2H
#pragma config BORV = VBOR_2P45 // Brown-out Reset Voltage Selection bits (Brown-out Reset Voltage (VBOR) set to 2.45V)
#pragma config ZCD = OFF        // ZCD Disable bit (ZCD disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config DEBUG = OFF      // Debugger Enable bit (Background debugger disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period selection bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT Disabled; SWDTEN is ignored

// CONFIG3H
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4L
#pragma config BBSIZE = BBSIZE_512// Boot Block Size selection bits (Boot Block size is 512 words)
#pragma config BBEN = OFF       // Boot Block enable bit (Boot block disabled)
#pragma config SAFEN = OFF      // Storage Area Flash enable bit (SAF disabled)
#pragma config WRTAPP = OFF     // Application Block write protection bit (Application Block not write protected)

// CONFIG4H
#pragma config WRTB = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-30000Bh) not write-protected)
#pragma config WRTC = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)
#pragma config WRTSAF = OFF     // SAF Write protection bit (SAF not Write Protected)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored)

// CONFIG5L
#pragma config CP = OFF         // PFM and Data EEPROM Code Protection bit (PFM and Data EEPROM code protection disabled)

// CONFIG5H

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

// F_OSC
#define _XTAL_FREQ 64000000


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include "uart.h"

void setBit(unsigned char *v, unsigned char i, unsigned char j){
  unsigned short m=0, n=0;
  unsigned char mask = 1;
  m = (i*160 + j)/8;
  n = (i*160 + j)%8;
  mask = mask << n;
  v[m] = v[m] | mask;
}

unsigned char getBit(unsigned char *v, unsigned char i, unsigned char j){
  unsigned short m=0, n=0;
  unsigned char mask = 1;
  m = (i*160 + j)/8;
  n = (i*160 + j)%8;
  mask = mask << n;
  return ((v[m] & mask) >> n);
}

unsigned char resetBit(unsigned char *v, unsigned char i, unsigned char j){
  unsigned short m=0, n=0;
  unsigned char mask = 1;
  m = (i*160 + j)/8;
  n = (i*160 + j)%8;
  mask = mask << n;
  mask = ~mask;
  v[m] = v[m] & mask;
}

/*----------------------------------------------INIT QUEUE----------------------------------------------*/

/*
 * Devido à restrição no uso de memória com a execução deste algoritmo, utilizamos uma fila baseada em array (ao invés 
 * de lista ligada), pois assim alocamos a matriz unidimensional da fila apenas uma vez, em sua inicialização na função
 * Flood Fill.
*/

/*typedef struct Queue {
    /*
     * Utilizamos inteiros que representam a frente da fila, a sua traseira, e seu tamanho, para que não sejam perdidos
     * durante a manipulação da fila.
    */
    /*int front, rear, size;
    int capacity; // Capacidade máxima da fila
    unsigned char arrayX[400]; // Ponteiro para a fila que armazena uma coordenada no eixo X da imagem.
    unsigned char arrayY[400]; // Idem, para o eixo Y.
} Queue;
*/

short int capacity, front, rear, size;
unsigned char arrayX[400];
unsigned char arrayY[400];
unsigned char visited[2400]; // Matriz que indica quais pixeis já foram visitados, em diferentes ocasiões.
//unsigned char img[2];
unsigned char img[2400]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,248,7,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,63,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,192,255,255,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,224,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,240,255,255,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
248,255,255,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,252,255,255,15,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,255,255,31,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,254,255,255,31,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,255,255,255,63,0,0,0,0,0,0,0,0,0,0,0,254,
7,0,0,0,255,255,255,63,0,0,0,0,0,0,0,0,0,0,192,255,31,0,0,0,
255,255,255,63,0,0,0,0,0,0,0,0,0,0,224,255,127,0,0,0,255,255,255,63,
0,0,0,0,0,0,0,0,0,0,240,255,255,0,0,0,255,255,255,63,0,0,0,0,
0,0,0,0,0,0,248,255,255,1,0,0,255,255,255,63,0,0,0,0,0,0,0,0,
0,0,252,255,255,1,0,0,255,255,255,63,0,0,0,0,0,0,0,0,0,0,252,255,
255,3,0,0,255,255,255,63,0,0,0,0,0,0,0,0,0,0,254,255,255,3,0,0,
255,255,255,63,0,0,0,0,0,0,0,0,0,0,254,255,255,7,0,0,255,255,255,31,
0,128,255,1,0,0,0,0,0,0,254,255,255,7,0,0,254,255,255,31,0,240,255,15,
0,0,0,0,0,0,255,255,255,7,0,0,254,255,255,15,0,252,255,31,0,0,0,0,
0,0,255,255,255,7,0,0,252,255,255,7,0,254,255,63,0,0,0,0,0,0,255,255,
255,7,0,0,248,255,255,3,0,255,255,127,0,0,0,0,0,0,254,255,255,7,0,0,
240,255,255,1,128,255,255,255,0,0,0,0,0,0,254,255,255,3,0,0,192,255,255,0,
192,255,255,255,1,0,0,0,0,0,254,255,255,3,0,0,0,255,63,0,192,255,255,255,
1,0,0,0,0,0,252,255,255,1,0,0,0,248,3,0,224,255,255,255,3,0,0,0,
0,0,252,255,255,1,128,127,0,0,0,0,224,255,255,255,3,0,0,0,0,0,248,255,
255,0,48,128,3,0,0,0,240,255,255,255,7,0,0,0,0,0,240,255,63,0,0,0,
6,0,0,0,240,255,255,255,7,0,0,0,0,0,192,255,31,0,6,12,12,0,0,0,
240,255,255,255,7,0,0,0,0,0,0,255,7,0,195,7,24,0,0,0,240,255,255,255,
7,0,0,0,0,0,0,0,0,128,241,255,48,0,0,0,240,255,255,255,7,0,0,0,
0,0,0,0,0,128,248,253,100,0,0,0,240,255,255,255,7,0,0,0,0,0,0,0,
0,64,244,255,111,0,0,0,240,255,255,255,7,0,0,0,0,0,255,0,0,64,240,255,
127,0,0,0,240,255,255,255,3,252,1,0,0,224,255,7,0,64,252,255,127,0,0,0,
224,255,255,255,131,255,15,0,0,248,255,31,0,96,248,255,223,0,0,0,224,255,255,255,
193,255,63,0,0,254,255,127,0,96,248,255,223,0,0,0,192,255,255,255,241,255,127,0,
0,255,255,255,0,96,248,255,127,0,0,0,128,255,255,255,248,255,255,0,128,255,255,255,
1,64,208,255,127,0,0,0,0,255,255,127,252,255,255,1,128,255,255,255,1,64,240,191,
127,0,0,32,0,254,255,31,252,255,255,1,192,255,255,255,3,192,224,127,47,0,0,255,
7,252,255,15,254,255,255,3,224,255,255,255,3,128,192,31,54,0,224,255,31,224,255,3,
254,255,255,3,224,255,255,255,7,128,129,7,18,0,248,255,127,0,28,0,255,255,255,7,
240,255,255,255,7,0,3,0,8,0,252,255,255,0,0,0,255,255,255,7,240,255,255,255,
7,0,14,0,6,0,254,255,255,1,0,0,255,255,255,7,240,255,255,255,7,0,28,0,
3,0,255,255,255,3,0,0,255,255,255,7,240,255,255,255,7,0,240,112,0,0,255,255,
255,7,0,0,255,255,255,7,240,255,255,255,7,0,0,0,0,128,255,255,255,7,0,0,
255,255,255,3,240,255,255,255,7,0,0,0,0,192,255,255,255,15,0,0,254,255,255,3,
240,255,255,255,7,0,0,0,0,192,255,255,255,15,0,0,254,255,255,3,240,255,255,255,
7,0,0,0,0,192,255,255,255,15,0,0,254,255,255,1,240,255,255,255,3,0,0,0,
0,192,255,255,255,31,0,0,252,255,255,0,224,255,255,255,3,128,255,0,0,224,255,255,
255,31,0,0,248,255,127,0,224,255,255,255,1,240,255,3,0,224,255,255,255,31,0,0,
240,255,63,0,192,255,255,255,0,252,255,15,0,224,255,255,255,15,0,0,192,255,31,0,
128,255,255,255,0,254,255,63,0,224,255,255,255,15,0,0,0,254,3,0,0,255,255,127,
0,255,255,127,0,192,255,255,255,15,0,0,0,0,0,0,0,254,255,31,128,255,255,255,
0,192,255,255,255,15,0,0,0,0,0,0,0,248,255,7,192,255,255,255,1,192,255,255,
255,15,0,0,0,0,0,0,0,224,255,1,224,255,255,255,1,128,255,255,255,7,0,0,
0,0,0,0,0,0,0,0,224,255,255,255,3,128,255,255,255,3,0,0,0,0,0,0,
0,0,0,0,240,255,255,255,3,0,255,255,255,1,0,0,0,0,0,0,0,0,0,0,
240,255,255,255,7,0,254,255,255,0,0,0,0,0,0,0,0,0,0,0,240,255,255,255,
7,0,252,255,127,0,0,0,0,0,0,0,0,0,0,0,248,255,255,255,7,0,240,255,
63,0,0,0,0,0,0,0,0,0,0,0,248,255,255,255,7,0,192,255,15,0,0,0,
0,0,0,0,0,0,0,0,248,255,255,255,7,0,0,254,0,0,0,0,0,0,0,0,
0,0,0,0,248,255,255,255,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
248,255,255,255,7,0,0,0,0,0,0,254,7,0,0,0,0,0,0,0,240,255,255,255,
3,0,0,0,0,0,192,255,31,0,0,0,0,0,0,0,240,255,255,255,3,0,0,0,
0,0,224,255,63,0,0,0,0,0,0,0,240,255,255,255,1,0,0,0,0,0,240,255,
255,0,0,0,0,0,0,0,224,255,255,255,1,0,0,0,0,0,248,255,255,1,0,0,
0,0,0,0,224,255,255,255,0,0,0,0,0,0,252,255,255,1,0,0,0,0,0,0,
192,255,255,127,0,0,0,0,0,0,254,255,255,3,0,0,0,0,0,0,128,255,255,63,
0,0,0,0,0,0,254,255,255,3,0,0,0,0,0,0,0,255,255,31,0,0,0,0,
0,0,254,255,255,7,0,0,0,0,0,0,0,254,255,7,0,0,0,0,0,0,255,255,
255,7,0,0,0,0,0,0,0,240,255,1,0,0,0,0,0,0,255,255,255,7,0,0,
0,0,0,0,0,0,31,0,0,0,0,0,0,0,255,255,255,7,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,255,255,255,7,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,255,255,255,7,0,0,0,0,0,0,0,0,0,224,7,0,0,0,
0,0,254,255,255,7,0,0,0,0,0,0,0,0,0,252,127,0,0,0,0,0,254,255,
255,3,0,0,0,0,0,0,0,0,0,255,255,1,0,0,0,0,254,255,255,3,0,0,
0,0,0,0,0,0,192,255,255,3,0,0,0,0,252,255,255,1,0,0,0,0,0,0,
0,0,224,255,255,15,0,0,0,0,248,255,255,0,0,0,0,0,0,0,0,0,240,255,
255,31,0,0,0,0,240,255,127,0,0,0,0,0,0,0,0,0,248,255,255,31,0,0,
0,0,224,255,63,0,0,0,0,0,0,0,0,0,252,255,255,63,0,0,0,0,128,255,
15,0,0,0,0,0,0,0,0,0,252,255,255,127,0,0,0,0,0,252,1,0,0,0,
0,0,0,0,0,0,254,255,255,127,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,254,255,255,127,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,
255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,
255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,127,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,255,255,127,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,254,255,255,63,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,254,255,255,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,252,255,255,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,248,255,
255,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,240,255,255,7,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,224,255,255,3,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,128,255,255,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,254,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,224,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/** @brief A função isFull determina se a fila passada por parâmetro está cheia ou não.
  * @param *queue Ponteiro para a fila
  * @return Retorna 1 se a fila está cheia, 0 caso contrário.
  */
int isFull () {
    return (size == capacity);
}


/** @brief A função isEmpty determina se a fila passada por parâmetro está vazia ou não.
  * @param *queue Ponteiro para a fila
  * @return Retorna 1 se a fila está vazia, 0 caso contrário.
  */
int isEmpty () {
    return (size == 0);
}

/** @brief A função push insere dois elementos, um na fila do eixo X, outro na fila do eixo Y.
  * @param *queue Ponteiro para a fila que se deseja fazer push.
  * @param X Inteiro a ser inserido em X.
  * @param Y Inteiro a ser inserido em Y.
  */
void push (unsigned char X, unsigned char Y) {
    if (isFull()) return;  // Não fazer nada se estiver cheia.
    rear = (rear+1) % capacity; // Array circular. A traseira é incrementada, módulo capacidade.
    arrayX[rear] = X; // Inserimos X em arrayX.
    arrayY[rear] = Y; // Inserimos Y em arrayY.
    size = size + 1; // Incrementamos o tamanho da fila.
}

/** @brief dequeue Retira da fila elementos na frente da fila. Retorna esses elementos por referência.
  * @param *x ponteiro para o elemento do eixo X a ser retornado
  * @param *y ponteiro para o elemento do eixo Y a ser retornado
  */
void dequeue (unsigned char *x, unsigned char *y) {
    if (isEmpty()) { // Não fazer nada se estiver vazia.
        return;
    }
    *x = arrayX[front]; // x recebe o valor que estava na frente da fila
    *y = arrayY[front]; // Idem para y.
    front = (front + 1)%capacity; // Incrementamos a fila em 1.
    size = size - 1; // Diminuimos em 1 o tamanho da fila.
}

/*------------------------------------------INIT FLOOD FILL-------------------------------------------*/

/** @brief A função isValid serve tanto para determinar se valores x e y estão dentro dos limites de
  * uma imagem 160x120, como para verificar se a posição indicada por esses dois números não foi já
  * utilizada, e para se certificar que o valor dessa posição é o mesmo valor que um certo comp.
  * @param binaryMatrix[120][160] Matriz binária representando a imagem passada a limiarização.
  * @param x Posição x na matriz
  * @param y Posição y na matriz
  * @param visited[120][160] Matriz que indica se cada uma de suas posições já foi visitada no Flood Fill 
  * @param comp Valor a ser comparado.
  */
int isValid (unsigned char binaryMatrix[2400], unsigned char x, unsigned char y, unsigned char visited[2400], unsigned char comp) {
    // Se x e y forem posições válidas, com um valor correto e que não tenham sido visitadas, retorne 1.
    if (x >= 0 && x < 120 && y >= 0 && y < 160 && (getBit(binaryMatrix,x,y) == comp) && (getBit(visited,x,y)==0)) {
        return 1;
    }
    return 0;
}

/** @brief A função erode aplica uma transformação de erosão na imagem, que consiste em tirar um pixel
  * do exterior de cada "objeto". Por exemplo:
  *                 0 0 0 0 0 0 0               0 0 0 0 0 0 0 
  *                 1 0 0 1 1 0 0               0 0 0 0 0 0 0     
  *                 0 0 1 1 1 1 0     ---->     0 0 0 1 1 0 0
  *                 0 0 1 1 1 1 0               0 0 0 1 1 0 0
  *                 0 0 0 1 1 0 0               0 0 0 0 0 0 0
  *                 0 0 0 0 0 0 0               0 0 0 0 0 0 0
  * Com a execução dessa função, conseguimos eliminar pixels individuais que fiquem "soltos" na imagem,
  * para que não sejam considerados uma componente conexa. É executada antes do dilate, para limpar a imagem.
  * @param outBinaryMatrix[120][160] Matriz resultante da operação, passada por referência.
  * @param visited[120][160] Matriz de visitados
  */
/*void erode (unsigned char outBinaryMatrix[2400], unsigned char visited[2400]) {
    unsigned char orBinaryMatrix[2400]; // Criamos uma matriz para guardar os valores originais da matriz binária.
    for (int i = 0; i < 2400; i++) {
        orBinaryMatrix[i] = outBinaryMatrix[i]; 
            /* Armazenamos os valores originais, para comparação posterior.
             * Isso é nececessário para que façamos a erosão sobre os pixels da imagem original, não da imagem
             * alterada pela erosão.
             */
    /*}
    for (int h = 0; h < 120; h++) {
        for (int w = 0; w < 160; w++) {
            if (getBit(orBinaryMatrix,h,w) == 1) {  // Se o pixel estiver "pintado"
                if ( (isValid(orBinaryMatrix, h, w-1, visited, 0) ||
                      isValid(orBinaryMatrix, h, w+1, visited, 0) || 
                      isValid(orBinaryMatrix, h+1, w, visited, 0) ||
                      isValid(orBinaryMatrix, h-1, w, visited, 0) //||
                      /* As verificações anteriores servem para determinar se algum dos vizinhos de um pixel branco
                       * (vizinhos apenas em cima e em baixo, esquerda e direita), é preto. Se for, o pixel atual
                       * fica preto, pois isso significa que estamos na borda de um objeto.
                       */
                /*)) {
		    resetBit(outBinaryMatrix,h,w);
                    //outBinaryMatrix[h][w] = 0;
                }
            }
        }
    }
}

/** @brief A função dilate é análoga à erode, fazendo, porém, o contrário. Ou seja, aplica uma transformação de
  * dilatação na imagem, que consiste em pintar de branco cada pixel do exterior imediato de cada "objeto". Exemplo:
  *                 0 0 0 0 0 0 0               1 0 0 0 0 0 0
  *                 1 0 0 0 0 0 0               1 1 0 1 1 0 0
  *                 0 0 0 1 1 0 0     ---->     1 0 1 1 1 1 0
  *                 0 0 0 1 1 0 0               0 0 1 1 1 1 0
  *                 0 0 0 0 0 0 0               0 0 0 1 1 0 0
  *                 0 0 0 0 0 0 0               0 0 0 0 0 0 0
  * Com a execução dessa função, conseguimos restaurar ao tamanho original cada objeto da imagem, após a execução do dilate.
  * @param outBinaryMatrix[120][160] Matriz resultante da operação, passada por referência.
  * @param visited[120][160] Matriz de visitados
  */
/*void dilate (unsigned char outBinaryMatrix[2400], unsigned char visited[2400]) {
    unsigned char orBinaryMatrix[2400]; // Criamos uma matriz para guardar os valores originais da matriz binária.
    for (int h = 0; h < 2400; h++) {
        orBinaryMatrix[h] = outBinaryMatrix[h];
            /* Armazenamos os valores originais, para comparação posterior.
             * Isso é nececessário para que façamos a erosão sobre os pixels da imagem original, não da imagem
             * alterada pela erosão.
             */
    /*}
    for (int h = 0; h < 120; h++) {
        for (int w = 0; w < 160; w++) {
            if (getBit(orBinaryMatrix,h,w) == 0) { // Se o pixel atual for preto.
                if (isValid(orBinaryMatrix, h, w-1, visited, 1) ||
                    isValid(orBinaryMatrix, h, w+1, visited, 1) ||
                    isValid(orBinaryMatrix, h+1, w, visited, 1) ||
                    isValid(orBinaryMatrix, h-1, w, visited, 1)// ||
                    /* As verificações anteriores servem para determinar se algum vizinho de um pixel preto é
                     * branco. Se for, pintamos o pixel atual de branco.
                     */
                /*) {
		    setBit(outBinaryMatrix,h,w);
                    //outBinaryMatrix[h][w] = 255;
                }
            }
        }
    }
}

/** @brief floodFill determina a area conectada a um dado pixel da imagem obtida.
  * Se o pixel vizinho é de foreground (255) este é preenchido com targetColor e 
  * inserido na fila sendo posteriormente marcado como visitado e removido da
  * fila ao verificar seus vizinhos. A função encerra quando a fila for vazia.
  *
  * @param binaryMatrix[120][160] matriz de pixels da imagem binária
  * @param x indica a posicão do pixel relativa às linhas da matriz
  * @param y indica a posição do pixel relativas às colunas da matriz
  * @param visited[120][160] visited[x][y] indica se o pixel (x,y) foi visitado (1) ou não (0)
  * @param targetColor inteiro entre 0 e 255 indicando uma cor em gray scale para preenchimento
  */
void floodFill (unsigned char binaryMatrix[2400], unsigned char x, unsigned char y, unsigned char visited[2400]/*, int targetColor*/) {
    capacity = 400;
    front = size=0;
    rear = capacity-1;
    push(x, y);
    unsigned char currentX = 0;
    unsigned char currentY = 0;
    while (!isEmpty()) {
        dequeue(&currentX, &currentY);//remove da fila e atualiza a posicao atual
        //binaryMatrix[currentX][currentY] = targetColor;//preenche o pixel atual com targetColor
        resetBit(binaryMatrix,currentX,currentY);
	setBit(visited,currentX,currentY);//marca o pixel como visitado
	//verifica vizinho acima, se ele for pixel de foreground e não
	//foi visitado é preenchido e inserido na fila
        if (isValid(binaryMatrix, currentX+1, currentY, visited, 1)){
            //binaryMatrix[currentX+1][currentY] = targetColor;
	    resetBit(binaryMatrix,currentX+1,currentY);
            push(currentX+1, currentY);
        }
	//verifica vizinho abaixo, análogo ao caso anterior
        if (isValid(binaryMatrix, currentX-1, currentY, visited, 1)){
            //binaryMatrix[currentX-1][currentY] = targetColor;
	    resetBit(binaryMatrix,currentX-1,currentY);
            push(currentX-1, currentY);
	}
	//verifica vizinho a direita, análogo ao caso anterior
        if (isValid(binaryMatrix, currentX, currentY+1, visited, 1)){
            //binaryMatrix[currentX][currentY+1] = targetColor;
	    resetBit(binaryMatrix,currentX,currentY+1);
            push(currentX, currentY+1);
        }
	//verifica vizinho a esquerda, análogo ao caso anterior
        if (isValid(binaryMatrix, currentX, currentY-1, visited, 1)){
            //binaryMatrix[currentX][currentY-1] = targetColor;
	    resetBit(binaryMatrix,currentX,currentY-1);
            push(currentX, currentY-1);
        }
    }
    
}



/**
 * @brief A função runAlgorithm executa todos os algoritmos já desenvolvidos. Primeiro é lido o caminho para um arquivo
 * pgm que se deseja que se execute as funções. Essa imagem é lida e armazenada na memória. Enquanto ela está sendo lida,
 * o histograma da imagem também está sendo gerado, histograma este que é posteriormente passado para a função Threshold.
 * Feito isso, e com o valor ótimo de limiarização obtido, gera-se uma imagem binária que posteriormente passa por erosão
 * e dilatação. Por fim, o Flood Fill é executado duas vezes, na primeira vez para se contar a quantidade de componentes
 * conexas, e na segunda para pintar essas componentes de forma a haver uma distribuição uniforme de cores entre todas
 * as componentes.
 * @return
 */
int runAlgorithm() {
   


    int i;
    for(i=0;i<2400;++i){
        visited[i]=0;
    }
   
    //erode(img, visited);         // Realizamos uma erosão para limpar pixels "soltos" na imagem.
    //dilate(img, visited);        // Em seguida, uma dilatação, para preservar o tamanho dos elementos.


    int connectedComps = 0;

    //unsigned char originalVisited[2400],originalImg[2400];
    /*for(i=0;i<2400;++i) {
	originalVisited[i] = visited[i];
	originalImg[i] = img[i];
    }*/

    for (int h = 0; h < 120; h++) {
        for (int w = 0; w < 160; w++) {
            if ((getBit(img,h,w)==1) && (getBit(visited,h,w)==0)) {    // Se o pixel atual for branco e não tiver sido visitado
                connectedComps++;                           // aumentamos a contagem de componentes
                floodFill(img, h, w, visited);       // e preenchemos aquela componente com uma cor qualquer.
            }
        }
    }
    char numcomps[10];
    sprintf(numcomps,"%d",connectedComps);
    UART_Escrever_Texto(numcomps);
    //printf("\nconnectedComps = %d\n", connectedComps);
    
    return 0;
}

int main() {
    int i = 0;
    //while (i<10) {
	//clock_t start = clock();
    runAlgorithm();
	//clock_t end = clock();
	//printf("tempo = %.4lf ms\n",(double)(end-start)/(CLOCKS_PER_SEC/1000));
	//++i;
    //}

    //for (int i = 0; i < 19200; i++) {
    //    printf("%d ,", 255);
    //}
}
