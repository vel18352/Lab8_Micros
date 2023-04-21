/*
Archivo:        Lab8_Vel18352
Dispositivo:    PIC16F887
Autor:		    Emilio Velasquez 18352
Compilador:	    XC8, MPLABX 5.40
Programa:       Comunicacion UART con potenciometro y barra led ASCII
Hardware:	    1 potenciometro, 1 barra led de 10 leds, 1 USB TTL
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
#include <stdbool.h>    
#include <stdlib.h>
#include <string.h>
#include "ADC.h"
#include "EUSART.h"         //Se incluyen las librerias necesarias
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
void ADC_Capture(void)                          //Capturar ADC y esperar
{
    if (ADCON0bits.GO)                          //Se chequea que no este ocurriendo una lectura de ADC
    {           
        return;
    }
    while(ADCON0bits.GO);                       //Espera a terminar la conversion ADC
    ADCON0bits.GO = 1;                          //Inicia la conversion ADC
}

void Texto(unsigned char *text)                 //Convertir una cadena de caracteres para enviar por UART
{
    while (*text != '\0')                       //Mientras el texto no se termine
    {       
    while (!TXIF);                              //Mientras la bandera no este activa
    TXREG = *text;                              //Envia caracter por TX
    *text++;                                    //Aumenta la posicion del caracter del string
    }
        
}

void Menu (void)                                //Funcion para escribir el menu
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
        ADC1 = ADRESH;                          //Lee ADC y almacena en ADC1 la lectura del primer potenciometro
        ADIF = 0;                               //Reinicia bandera ADC
    }
    
    if (PIR1bits.RCIF)                          //Chequea bandera de interrupcion de RX
    {
        Selector = RCREG;                       //Recibe variable del menu
        if (Selector == 0x61)                   //En caso de ser "a" lee potenciometro
        {       
            itoa(Potentiometer,ADC1,10);        //Convertir int to string, primer parametro el valor a devolver, segundo parametro el valor a convertir, tercer la base
            Texto("El valor del potenciometro es: \n"); 
            Texto(Potentiometer);               //Llama a funcion Texto() para escribir lo correspondiente en TX            
        }
        else if (Selector == 0x62)              //En caso de ser "b" espera a recibir un caracter ASCII para escribir en puerto B
        {
            Texto("Ingrese caracter ASCII para mostrar: \n");   
            while (!PIR1bits.RCIF);             //Espera a recibir el caracter
            PORTB = RCREG;                      //Escribe en puerto B
        }
        Menu();                                 //Llama a menu para mostrarlo de nuevo
    }
            
    if (PIE1bits.TXIE && PIR1bits.TXIF)         //En caso de interrupcion de TX este encendida y habilitada
    {
        if (Menu_Flag == false)         
        {
            Menu();                             //Muestra el menu inicial
            Menu_Flag = true;                   //Conmuta para no mostrar mmas de una vez el menu
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
    
    // Configuraci�n del oscilador
    OSCCONbits.IRCF = 0b0110;   // 4 MHz
    OSCCONbits.SCS = 1;         // Oscilador interno
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
    Setup();                            //Setup de puertos y oscilador
    Int_Enable();                       //Habilita interrupciones
    ADC_Int(0);                         //Configuracion ADC
    ADC_Select(0);                      //Canal 0 del ADC                    
    EUSART_Init();                      //Inicia y configura UART
    EUSART_Enable_Isr();                //Habilita interrupciones UART
    
    while(1)                           //Loop principal
    {
        ADC_Capture();                 //Llama a la funcion para capturar ADC   
    }
    return;
}