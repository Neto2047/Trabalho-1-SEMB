
#ifndef UART_H
#define	UART_H

/*  
 https://www.microchip.com/forums/m1071166.aspx
 */

/*
 * Esta função configura os registradores para usar a interface EUSART do MCU, configurando os pinos 
 * RC6 como TX e RC7 como RX. O baudrate é fixo de 9600bps.
 */
void UART_iniciar(){
    //Pines para UART (remapping))
    RC6PPS = 0x13; //RC6->UART1:TX1; 
    U1RXPPS = 0x17; //RC7->UART1:RX1; 
    
    ANSELC7 = 0; // RC7 como E/S Digital, RX.
    ANSELC6 = 0; // RC6 como E/S Digital, TX.
    TRISC7 = 1; // RC7 as input.
    TRISC6 = 0; // RC6 as output.
    // Disable interrupts
    PIE3bits.U1RXIE = 0;
    PIE3bits.U1IE = 0;
    // Setup
    U1CON0 = 0xB0; //High speed, transmitter enabled, receiver enabled, Asynchronous 8-bit UART mode.
    U1CON1 = 0x80; //Serial Port enabled, 0x80;
    U1CON2 = 0x00;
// U1BRG =  (F_OSC)/( 4 * baud_rate) 
    U1BRGH = 0x06; //Baudrate 9600 -> U1BRG = 1666 (0x0682)
    U1BRGL = 0X82; 

    //Error Registers
    U1ERRIR = 0x00;
    U1ERRIE = 0x00;
    U1UIR = 0x00;
    U1FIFO = 0x00;
    //Parameters
    U1P1H = 0x00; //Not used
    U1P1L = 0x00; //Not used
    U1P2H = 0x00; //Not used
    U1P2L = 0x00; //Not used
    U1P3H = 0x00; //Not used
    U1P3L = 0x00; //Not used
    // Checksum
    U1TXCHK = 0x00;
    U1RXCHK = 0x00;
    PIE3bits.U1RXIE = 1;
    PIE3bits.U1IE = 1;
    //U1CON1bits.ON = 1;
    
    /*
    ANSELCbits.ANSC7 = 0;   //Desativa função analógica no pino RC7
    ANSELCbits.ANSC6 = 0;   //Desativa função analógica no pino RC6
    RC6PPS = 0x10;          //Configura pino RC6 como TX
    RXPPSbits.RXPPS = 0x17; //Configura pino RC7 como RX
    TX1STAbits.BRGH = 0;    //Necessário para configurar o baud rate da EUSART
    BAUDCON1bits.BRG16 = 0; //Configura registrado BAUDCON1 como 8 bits
    SPBRG = 51;             //Configura baud rate para 9600bps
    RC1STAbits.SPEN = 1;    //Habilita EUSART
    RC1STAbits.CREN = 1;    //Habilita recebimento EUSART (RX)
    TX1STAbits.TXEN = 1;    //Habilita transmissão EUSART (TX)
    TX1STAbits.SYNC = 0;    //Configura EUSART para modo assíncrono
     */
    
}
// calculo do taxa de transmissão serial para Uart
// F_CPU = 32000000/64
// SPBRG =  (((float)(F_CPU)/(float)baud_rate)-1) 

//Esta função serve para ler um simples byte na pino RC7 (RX).
char UART_Ler(){
    while(0 == PIR3bits.U1RXIF);
    //while(!RCIF);
        return(U1RXB);
    //return RCREG;
}

/*
 * Esta função pode ler um conjunto de caracteres enviados serialmente para o pino RC7, serve para ler os comandos
 * enviados pelo computador através do script em python. A regra é que cada comando contenha um caractere '$' 
 * ao final, representando que o comando acabou.
 */
void UART_Ler_Texto(char* saida){
    short index = 0;
    char c;
    while(c != '$'){
       c = UART_Ler(); 
       saida[index] = c;
       index++;
    }
    saida[index - 1] = '\0';
}

//Esta função serve para escrever um byte no pino RC6 (TX) do MCU.
void UART_Escrever(char data){
    
    while(0 == PIR3bits.U1TXIF);
    U1TXB = data;

}


/*
 * Esta função serve para escrever um conjunto de caracteres no pino RC6 (TX), não foi usada na aplicação,
 * entretanto pode ser útil para outras aplicações.
 */
void UART_Escrever_Texto(char* texto){
    int index;
    for(index=0; texto[index]!='\0'; index++)
        UART_Escrever(texto[index]);
}

#endif	/* UART_H */

