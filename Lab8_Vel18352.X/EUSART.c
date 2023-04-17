#include <xc.h>

void EUSART_Init(void)
{ 
    SPBRGH = 0;
    SPBRG = 25;
    
    RCSTAbits.SPEN = 1;
    
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;         // Habilitar transmisor y receptor
    
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    TXSTAbits.SENDB = 1;
    
    TXSTAbits.TX9 = 0;
    RCSTAbits.RX9 = 0;          // 8 bits
    

}

void EUSART_Enable_Isr(void)
{
    PIE1bits.RCIE = 1;
    PIE1bits.TXIE = 1;
}

void EUSART_Disable_Isr(void)
{
    PIE1bits.TXIE = 0;
    PIE1bits.RCIE = 0;
}
