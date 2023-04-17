#ifndef __EUSART__
#define	__EUSART__

#include <xc.h> 
#include <stdint.h>


void EUSART_Init(void);

void EUSART_Enable_Isr(void);

void EUSART_Disable_Isr(void);

#endif