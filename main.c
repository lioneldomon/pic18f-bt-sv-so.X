


#include <xc.h>
#include <stdio.h>
#include <pic18f25k22.h>
#include "file.h"
#include "uart.h"
#include "i2c.h"
#include "maitre.h"

/**
 * Bits de configuration:
 */
#pragma config FOSC = INTIO67  // Osc. interne, A6 et A7 comme IO.
#pragma config IESO = OFF      // Pas d'osc. au démarrage.
#pragma config FCMEN = OFF     // Pas de monitorage de l'oscillateur.

// Nécessaires pour ICSP / ICD:
#pragma config MCLRE = EXTMCLR // RE3 est actif comme master reset.
#pragma config WDTEN = OFF     // Watchdog inactif.
#pragma config LVP = OFF       // Single Supply Enable bits off.

#define CAPTURE_FLANC_MONTANT 1
#define CAPTURE_FLANC_DESCENDANT 0

#ifndef TEST



void low_priority interrupt bassePriorite() {

    if (PIR1bits.TX1IF) {
        uartTransmission();
//        PIR1bits.TX1IF = 0;
    }
    if (PIR1bits.RC1IF) {
        uartReception();
    }
}

/**
 * Initialise les périphériques.
 */
void initialiseHardware() {
        
    OSCCONbits.IRCF = 0b110;
    
    initialiseUART1();
    
    initialisationFC114();



    // Désactive les entrées analogiques
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    TRISBbits.RB0 = 0;

    
    // Active les interruptions (basse priorité):
    PIE1bits.TX1IE = 1;
    IPR1bits.TX1IP = 0;
    PIE1bits.RC1IE = 1;
    IPR1bits.RC1IP = 0;
    
    // Active les interruptions générales:
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
}

/**
 * Point d'entrée du programme.
 */
void main(void) {
    char buffer[50];
    initialiseHardware();
    uartReinitialise();
    maitreMain();
    // Affiche les caractères de la file.
    while(1) {
//        gets(buffer);
//        printf("Vous avez dit: %s",buffer);
    }
}
#endif

#ifdef TEST

/**
 * Point d'entrée des tests unitaires.
 */
void main() {
    
    initialiseTests();
    
    testFile();
    testMorse();
    
    finaliseTests();
    
    while(1);
}
#endif
