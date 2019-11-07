/*
 * Arquivo: main.c
 * Autores: Luis Filipe de Lima Sales (GitHub @luisfilipels) e Raimundo Azevedo (GitHub @Neto2047)
 *
 * Descrição: Este algoritmo consiste na execução do algoritmo Flood Fill sobre uma imagem PGM (em formato binário),
 * passando por, antes disso, pelo processamento da imagem pelo algoritmo de Otsu (para determinação do nível ótimo de
 * limiarização), por uma roodada de erosão e outra de dilatação, e por fim, pelo Flood Fill em si, que é utilizado para
 * a contagem de componentes conexas que foram obtidas a partir da imagem binária obtida pelo algoritmo de Otsu. A imagem
 * é, por fim, exportada para outro arquivo, out.pgm, que mostra o resultado dessas operações.
*/

/*
 * HISTÓRICO DE EDIÇÕES:
 * -------------------------------------------------
 * Data: 8/10/19, 7:22 PM
 * Autor: Luis Filipe e Raimundo Azevedo
 * Motivo: Primeiro commit no Git. Base do código, e implementação de Otsu, implmentados dias antes.
 * -------------------------------------------------
 * Data: 9/10/19, 11:44 PM
 * Autor: Luis Filipe
 * Motivo: Flood Fill funcionando.
 * -------------------------------------------------
 * Data: 10/10/19, 8:14 PM
 * Autor: Luis Filipe e Raimundo Azevedo
 * Motivo: Comentários no código.
 * -------------------------------------------------
 * Data: 11/10/19, 2:47 PM
 * Autor: Luis Filipe
 * Motivo: Correções no algoritmo.
 * -------------------------------------------------
 * Data: 16/10/19, 8:16 PM
 * Autor: Raimundo Azevedo
 * Motivo: Lógica equivalente para a função erode.
 * -------------------------------------------------
 * Data: 16/10/19, 10:22 PM
 * Autor: Raimundo Azevedo
 * Motivo: Função Threshold melhorada. Adiçao de comentários em Otsu, Threshold e Flood Fill.
 * -------------------------------------------------
 * Data: 17/10/19, 10:42 PM
 * Autor: Luis Filipe
 * Motivo: Mais comentários. Algoritmo OK!
 * -------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>

void setBit(uint8_t *v, uint8_t i, uint8_t j){
  uint16_t m=0, n=0;
  uint8_t mask = 1;
  m = (i*160 + j)/8;
  n = (i*160 + j)%8;
  mask = mask << n;
  v[m] = v[m] | mask;
}

uint8_t getBit(uint8_t *v, uint8_t i, uint8_t j){
  uint16_t m=0, n=0;
  uint8_t mask = 1;
  m = (i*160 + j)/8;
  n = (i*160 + j)%8;
  mask = mask << n;
  return ((v[m] & mask) >> n);
}

uint8_t resetBit(uint8_t *v, uint8_t i, uint8_t j){
  uint16_t m=0, n=0;
  uint8_t mask = 1;
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

typedef struct Queue {
    /*
     * Utilizamos inteiros que representam a frente da fila, a sua traseira, e seu tamanho, para que não sejam perdidos
     * durante a manipulação da fila.
    */
    int front, rear, size;
    int capacity; // Capacidade máxima da fila
    uint8_t arrayX[400]; // Ponteiro para a fila que armazena uma coordenada no eixo X da imagem.
    uint8_t arrayY[400]; // Idem, para o eixo Y.
} Queue;


/** @brief A função isFull determina se a fila passada por parâmetro está cheia ou não.
  * @param *queue Ponteiro para a fila
  * @return Retorna 1 se a fila está cheia, 0 caso contrário.
  */
int isFull (Queue *queue) {
    return (queue->size == queue->capacity);
}


/** @brief A função isEmpty determina se a fila passada por parâmetro está vazia ou não.
  * @param *queue Ponteiro para a fila
  * @return Retorna 1 se a fila está vazia, 0 caso contrário.
  */
int isEmpty (Queue *queue) {
    return (queue->size == 0);
}

/** @brief A função push insere dois elementos, um na fila do eixo X, outro na fila do eixo Y.
  * @param *queue Ponteiro para a fila que se deseja fazer push.
  * @param X Inteiro a ser inserido em X.
  * @param Y Inteiro a ser inserido em Y.
  */
void push (Queue *queue, uint8_t X, uint8_t Y) {
    if (isFull(queue)) return;  // Não fazer nada se estiver cheia.
    queue->rear = (queue->rear+1) % queue->capacity; // Array circular. A traseira é incrementada, módulo capacidade.
    queue->arrayX[queue->rear] = X; // Inserimos X em arrayX.
    queue->arrayY[queue->rear] = Y; // Inserimos Y em arrayY.
    queue->size = queue->size + 1; // Incrementamos o tamanho da fila.
}

/** @brief dequeue Retira da fila elementos na frente da fila. Retorna esses elementos por referência.
  * @param *x ponteiro para o elemento do eixo X a ser retornado
  * @param *y ponteiro para o elemento do eixo Y a ser retornado
  */
void dequeue (Queue *queue, uint8_t *x, uint8_t *y) {
    if (isEmpty(queue)) { // Não fazer nada se estiver vazia.
        return;
    }
    *x = queue->arrayX[queue->front]; // x recebe o valor que estava na frente da fila
    *y = queue->arrayY[queue->front]; // Idem para y.
    queue->front = (queue->front + 1)%queue->capacity; // Incrementamos a fila em 1.
    queue->size = queue->size - 1; // Diminuimos em 1 o tamanho da fila.
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
int isValid (uint8_t binaryMatrix[2400], uint8_t x, uint8_t y, uint8_t visited[2400], uint8_t comp) {
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
void erode (uint8_t outBinaryMatrix[2400], uint8_t visited[2400]) {
    uint8_t orBinaryMatrix[2400]; // Criamos uma matriz para guardar os valores originais da matriz binária.
    for (int i = 0; i < 2400; i++) {
        orBinaryMatrix[i] = outBinaryMatrix[i]; 
            /* Armazenamos os valores originais, para comparação posterior.
             * Isso é nececessário para que façamos a erosão sobre os pixels da imagem original, não da imagem
             * alterada pela erosão.
             */
    }
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
                )) {
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
void dilate (uint8_t outBinaryMatrix[2400], uint8_t visited[2400]) {
    uint8_t orBinaryMatrix[2400]; // Criamos uma matriz para guardar os valores originais da matriz binária.
    for (int h = 0; h < 2400; h++) {
        orBinaryMatrix[h] = outBinaryMatrix[h];
            /* Armazenamos os valores originais, para comparação posterior.
             * Isso é nececessário para que façamos a erosão sobre os pixels da imagem original, não da imagem
             * alterada pela erosão.
             */
    }
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
                ) {
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
void floodFill (uint8_t binaryMatrix[2400], int8_t x, int y, uint8_t visited[2400]/*, int targetColor*/) {
    Queue queue;
    queue.capacity = 400;
    queue.front = queue.size=0;
    queue.rear = queue.capacity-1;
    push(&queue, x, y);
    uint8_t currentX = 0;
    uint8_t currentY = 0;
    while (!isEmpty(&queue)) {
        dequeue(&queue, &currentX, &currentY);//remove da fila e atualiza a posicao atual
        //binaryMatrix[currentX][currentY] = targetColor;//preenche o pixel atual com targetColor
        resetBit(binaryMatrix,currentX,currentY);
	setBit(visited,currentX,currentY);//marca o pixel como visitado
	//verifica vizinho acima, se ele for pixel de foreground e não
	//foi visitado é preenchido e inserido na fila
        if (isValid(binaryMatrix, currentX+1, currentY, visited, 1)){
            //binaryMatrix[currentX+1][currentY] = targetColor;
	    resetBit(binaryMatrix,currentX+1,currentY);
            push(&queue, currentX+1, currentY);
        }
	//verifica vizinho abaixo, análogo ao caso anterior
        if (isValid(binaryMatrix, currentX-1, currentY, visited, 1)){
            //binaryMatrix[currentX-1][currentY] = targetColor;
	    resetBit(binaryMatrix,currentX-1,currentY);
            push(&queue, currentX-1, currentY);
	}
	//verifica vizinho a direita, análogo ao caso anterior
        if (isValid(binaryMatrix, currentX, currentY+1, visited, 1)){
            //binaryMatrix[currentX][currentY+1] = targetColor;
	    resetBit(binaryMatrix,currentX,currentY+1);
            push(&queue, currentX, currentY+1);
        }
	//verifica vizinho a esquerda, análogo ao caso anterior
        if (isValid(binaryMatrix, currentX, currentY-1, visited, 1)){
            //binaryMatrix[currentX][currentY-1] = targetColor;
	    resetBit(binaryMatrix,currentX,currentY-1);
            push(&queue, currentX, currentY-1);
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
   uint8_t img[2400]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
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

    uint8_t visited[2400]; // Matriz que indica quais pixeis já foram visitados, em diferentes ocasiões.
    int i;
    for(i=0;i<2400;++i){
	visited[i]=0;
    }
   
    erode(img, visited);         // Realizamos uma erosão para limpar pixels "soltos" na imagem.
    dilate(img, visited);        // Em seguida, uma dilatação, para preservar o tamanho dos elementos.


    int connectedComps = 0;

    uint8_t originalVisited[2400],originalImg[2400];
    for(i=0;i<2400;++i) {
	originalVisited[i] = visited[i];
	originalImg[i] = img[i];
    }

    for (int h = 0; h < 120; h++) {
        for (int w = 0; w < 160; w++) {
            if ((getBit(img,h,w)==1) && (getBit(visited,h,w)==0)) {    // Se o pixel atual for branco e não tiver sido visitado
                connectedComps++;                           // aumentamos a contagem de componentes
                floodFill(img, h, w, visited);       // e preenchemos aquela componente com uma cor qualquer.
            }
        }
    }
    printf("\nconnectedComps = %d\n", connectedComps);
    
    return 0;
}

int main() {
    int i = 0;
    //while (i<10) {
	clock_t start = clock();
        runAlgorithm();
	clock_t end = clock();
	printf("tempo = %.4lf ms\n",(double)(end-start)/(CLOCKS_PER_SEC/1000));
	++i;
    //}

    //for (int i = 0; i < 19200; i++) {
    //    printf("%d ,", 255);
    //}
}

