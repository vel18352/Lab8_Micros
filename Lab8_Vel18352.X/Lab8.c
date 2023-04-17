/*
Archivo:        Lab6_Vel18352
Dispositivo:    PIC16F887
Autor:		    Emilio Velasquez 18352
Compilador:	    XC8, MPLABX 5.40
Programa:       Contador binario y display de voltaje por ADC
Hardware:	    2 potenciometros, 8 leds, 4 displays 7 segmentos
Creado:         27/03/2023
Ultima modificacion: 27/03/2023
 */
//******************************************************************************
// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
//******************************************************************************

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT
#pragma config WDTE = OFF       
#pragma config PWRTE = OFF      
#pragma config MCLRE = OFF      
#pragma config CP = OFF         
#pragma config CPD = OFF        
#pragma config BOREN = OFF      
#pragma config IESO = OFF       
#pragma config FCMEN = OFF      
#pragma config LVP = OFF        
// CONFIG2
#pragma config BOR4V = BOR40V   
#pragma config WRT = OFF        
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//******************************************************************************
// Includes
//******************************************************************************
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>    //Se incluyen las librerias necesarias
#include <stdlib.h>
#include <string.h>

#include "ADC.h"
#include "EUSART.h"
//******************************************************************************
// Definiciones
//******************************************************************************
#define _XTAL_FREQ 4000000  //Se establece el oscilador a 4.0Mhz

//******************************************************************************
// Variables
//******************************************************************************
uint8_t ADC1 = 0;
uint8_t Selector = 0;
bool Menu_Flag = false;

char Potentiometer[];


//******************************************************************************
// Subrutinas
//******************************************************************************
void ADC_Capture(void)                           //Conmutar canal el cual se le hara lectura de ADC para ambos potenciometros
{
    if (ADCON0bits.GO)                          //Se chequea que no este ocurriendo una lectura de ADC
    {           
        return;
    }
    while(ADCON0bits.GO);                       //Espera a terminar la conversion ADC
    ADCON0bits.GO = 1;                          //Inicia la conversion ADC
}

void Texto(unsigned char *text)
{
    while (*text != '\0')
    {
    while (!TXIF);
    TXREG = *text;
    *text++;
    }
        
}

void Menu (void)
{
    Texto("\n Menu, Seleccione opcion \r\n");
    Texto("a. Leer potenciometro \r\n");
    Texto("b. Enviar ASCII \r\n");
    
}
//******************************************************************************
// Interrupciones
//******************************************************************************
void __interrupt() isr(void)
{
    if (ADIF)                                   //Chequea bandera de interrupcion de ADC
    {       
        ADC1 = ADRESH;                           //Lee ADC y almacena en ADC1 la lectura del primer potenciometro
        ADIF = 0;                               //Reinicia bandera ADC
    }
    
    if (PIR1bits.RCIF)
    {
        Selector = RCREG;
        if (Selector == 0x61)
        {
            itoa(Potentiometer,ADC1,10);
            Texto("El valor del potenciometro es: \n");
            Texto(Potentiometer);
            
        }
        else if (Selector == 0x62)
        {
            Texto("Ingrese caracter ASCII para mostrar: \n");
            while (!PIR1bits.RCIF);
            PORTB = RCREG;
        }
        Menu();
    }
    
    if (PIE1bits.TXIE && PIR1bits.TXIF)
    {
        if (Menu_Flag == false)
        {
            Menu();            
            Menu_Flag = true;
        }
    }
}

//******************************************************************************
// Configuraciones
//******************************************************************************
void Setup(void)
{
    ANSEL  = 0x01;  //Enciende primeros 2 canales del ADC
    ANSELH = 0x00;   
    TRISA = 0x01;   //Establece primeros dos bits de PORTA como entrada
    PORTA = 0;      //Limpia PORTA
    TRISB = 0;      //Establece PORTB como salida
    PORTB = 0;      //Limpia PORTB
    TRISC = 0;      //Establece PORTC como salida
    PORTC = 0;      //Limpia PORTC
    TRISD = 0;      //Establece PORTD como salida
    PORTD = 0;      //Limpia PORTD
    
    // Configuración del oscilador
    OSCCONbits.IRCF = 0b0110;   // 4 MHz
    OSCCONbits.SCS = 1;         // Oscilador intern
}

void Int_Enable(void)
{
    ADIF = 0;
    ADIE = 1;   //Habilitar interrupciones ADC
    PEIE = 1;   //Habilitar interrupciones Perifericas
    GIE = 1;    //Habilitar interrupciones Globales
}


//******************************************************************************
// Main Loop
//******************************************************************************
void main(void) 
{
    Setup();        
    Int_Enable();
    ADC_Int(0); 
    ADC_Select(0);                              
    EUSART_Init();
    EUSART_Enable_Isr(); 
    
    while(1)                                    //Loop principal
    {
        ADC_Capture();                           //Llama a la funcion de conmutar el canal de ADC    
    }
    return;
}