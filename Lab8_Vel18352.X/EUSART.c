#include <xc.h>

void EUSART_Init(void)
{ 
    SPBRGH = 0;
    SPBRG = 25;                 //Se establece a 9600 baudrate a 4Mhz de velocidad de oscilador
    
    RCSTAbits.SPEN = 1;         //Se habilitan los pines para comunicacion UART
    
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;         //Habilitar transmisor y receptor
    
    TXSTAbits.SYNC = 0;         //Se establece el modo asíncrono de comunicación
    TXSTAbits.BRGH = 1;         //Se establece el modo de alta velocidad de transmisión
    TXSTAbits.SENDB = 1;        //Habilita la detección de bits de sincronización adicionales en la transmisión de datos
    
    TXSTAbits.TX9 = 0;
    RCSTAbits.RX9 = 0;          //8 bits de envio y recepcion de datos
}

void EUSART_Enable_Isr(void)
{
    PIE1bits.RCIE = 1;
    PIE1bits.TXIE = 1;          //Habilita interrupciones UART
}

void EUSART_Disable_Isr(void)
{
    PIE1bits.TXIE = 0;
    PIE1bits.RCIE = 0;          //Desabilitar interrupciones UART
}
