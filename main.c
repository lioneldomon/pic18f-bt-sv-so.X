


#include <xc.h>
#include <stdio.h>
#include <pic18f25k22.h>

#include <stdlib.h>
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
    
    if (PIR1bits.ADIF) {
    //    i2cPrepareCommandePourEmission(I2cAdresse, ADRESH);
        PIR1bits.ADIF = 0;
    }
    
    if (PIR1bits.TMR1IF) {
        TMR1 = 3035;
    //    i2cPrepareCommandePourEmission(LECTURE_POTENTIOMETRE, 0);
        PIR1bits.TMR1IF = 0;
    }

    if (PIR1bits.SSP1IF) {
        i2cMaitre();
        
        PIR1bits.SSP1IF = 0;
    }
}

/**
 * Initialise les périphériques.
 */
void initialiseHardware() {
        
    OSCCONbits.IRCF = 0b110;
    
    initialiseUART1();
    
    initialisationFC114();

    
    // Active le MSSP1 en mode Maître I2C:
    TRISCbits.RC3 = 1;      // RC3 comme entrée...
    ANSELCbits.ANSC3 = 0;   // ... digitale.
    TRISCbits.RC4 = 1;      // RC4 comme entrée...
    ANSELCbits.ANSC4 = 0;   // ... digitale.

    SSP1CON1bits.SSPEN = 1;     // Active le module SSP.
    
    SSP1CON3bits.PCIE = 1;      // Active l'interruption en cas STOP.
    SSP1CON3bits.SCIE = 1;      // Active l'interruption en cas de START.
    SSP1CON1bits.SSPM = 0b1000; // SSP1 en mode maître I2C.
    SSP1ADD = 3;                // FSCL = FOSC / (4 * (SSP1ADD + 1)) = 62500 Hz.

    PIE1bits.SSP1IE = 1;        // Interruption en cas de transmission I2C...
    IPR1bits.SSP1IP = 0;        // ... de basse priorité.


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

void retourValeurEsclave(unsigned char adresse, unsigned char valeur) {
    switch(adresse) {
        case SONAR_LEC:
            printf("Distance: %d \r\n", valeur);
            break;

        default:
            Nop();
    }
}

void printMenu(){
    printf("Liste de commande: \r\n");
    printf("1 : Distance sonar \r\n");
    printf("2 : Controle des servomoteurs \r\n");
    printf("0 : Apercu des commandes \r\n");    
}
/**
 * Point d'entrée du programme.
 */
void main(void) {
    char buffer[50];
    char c =0;
    initialiseHardware();
    uartReinitialise();
    i2cRappelCommande(retourValeurEsclave);
    i2cReinitialise();
    printMenu();
    // Affiche les caractères de la file.
    while(1) {

        gets(buffer);
        switch(buffer[0]){
            
            case '1':
                i2cPrepareCommandePourEmission(SONAR_LEC, 0);
                break;
            case '2':
                printf("Controle du servomoteur: \r\n");
                printf("Entrer une position entre 0 et 255 : ");
                gets(buffer);
                c = (char) atoi(buffer);
                i2cPrepareCommandePourEmission(ECRITURE_SERVO_0, c);

                break;
            case '0':
                printMenu();
                break;
            default:
                printMenu();
        }
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
