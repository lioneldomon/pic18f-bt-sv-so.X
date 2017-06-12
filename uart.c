#include <xc.h>
#include <stdio.h>
#include "file.h"
#include "uart.h"
/**
 * Fonction qui transmet un caractère à la EUSART.
 * Il s'agit de l'implémentation d'une fonction système qui est
 * appelée par <code>printf</code>.
 * 
 * Cette implémentation envoie le caractère à la UART. Si un terminal
 * est connecté aux sorties RX / TX, il affichera du texte.
 * @param data Le code ASCII du caractère à afficher.
*/
//void putch(char data) {
//    while( ! TX1IF);
//    TXREG1 = data;
//}

/**
 * File des caractères reçus.
 * La file est peuplée par {@link #uartReception} et
 * vidée par {@link #uartGetCh}.
 */
File fileReception;

/**
 * File des caractères à transmettre.
 * La file est peuplée par {@link #uartPutch} et
 * vidée par {@link uartTransmission}.
 */
File fileTransmission;

typedef enum {
    UART_TRANSMISSION_EN_REPOS,
    UART_TRANSMISSION_EN_COURS
} UartStatus;

/**
 * État actuel de la transmission.
 */
UartStatus uartEtat = UART_TRANSMISSION_EN_REPOS;

/**
 * Ajoute un caractère à la file de transmission.
 * Si la file de transmission est pleine, la méthode attend qu'il
 * y ait de la place.
 * @param data Le caractère à placer.
 */
void uartPutch(char data) {
    if (uartEtat == UART_TRANSMISSION_EN_REPOS) {
        uartEtat = UART_TRANSMISSION_EN_COURS;
        TXREG1 = data;
        PIE1bits.TX1IE = 1;
    } else {
        while (fileEstPleine(&fileTransmission));
        fileEnfile(&fileTransmission, data);            
    }
}

/**
 * Indique qu'il n'y a plus de caractères à transmettre.
 * @return 255 si il n'y a plus de caractères à transmettre.
 * @see uartTransmission
 */
unsigned char uartCaracteresDisponiblesPourTransmission() {
    if (fileEstVide(&fileTransmission)) {
        uartEtat = UART_TRANSMISSION_EN_REPOS;
        return 0;
    } else {
        return 255;
    }
}

/**
 * Récupère un caractère de la file de réception.
 * Si la file de réception est vide, la méthode attend qu'un
 * caractère soit disponible.
 * @return Un caractère de la file de réception
 */
char uartGetch() {
    unsigned char n = 0;
    while(fileEstVide(&fileReception)) {
        n++;
    };
    return fileDefile(&fileReception);  
}

/**
 * Fonction à appeler chaque fois que la UART reçoit un caractère.
 * Cette fonction est normalement appelée depuis la routine de gestion
 * des interruptions, en réponse à une interruption de réception.
 * <pre>
 * void interrupt interruptions() {
 *  if (PIR1bits.RC1IF) {
 *      uartReception();
 *  }
 * }
 * </pre>
 * @param c Le caractère reçu.
 */
void uartReception() {
    char c = 0;
    fileEnfile(&fileReception, RCREG1);     // Sauve la valeur recu dans la file de réception
    
    // Es que le dernier caractères est le CR?
    if(RCREG == 0x0D){ // Détection du CR
        printf("Vous avez dit :  \r\n");
        while(fileEstVide(&fileReception)==0){
            printf("%c",fileDefile(&fileReception));            
        }
        printf("\r\n");

//        }
        fileReinitialise(&fileReception);           // Init file de reception
    }

}

/**
 * Fonction à appeler pour récupérer le prochain caractère à transmettre
 * à travers la UART.
 * Cette fonction est typiquement appelée depuis la routine de transmission
 * des interruptions, en réponse à une interruption de transmission.
 * <pre>
 * void interrupt interruptions() {
 *  if (IPR1bits.TX1IF) {
 *      uartTransmission();
 *  }
 * }
 * </pre>
 * @return Le caractère à envoyer.
 */
void uartTransmission() {
    if (uartCaracteresDisponiblesPourTransmission()) {
        TXREG1 = fileDefile(&fileTransmission);
    } else {
        PIE1bits.TX1IE = 0;
    }
}

/**
 * Réinitialise la UART.
 */
void uartReinitialise() {
    fileReinitialise(&fileReception);           // Init file de reception
    fileReinitialise(&fileTransmission);        // Init file de transmission
    uartEtat = UART_TRANSMISSION_EN_REPOS;
}

#ifndef TEST
/**
 * Implémentation d'une fonction système qui est appelée par <code>printf</code>.
 * Cette implémentation ajoute le caractère à la file de transmission.
 * @param data Le code ASCII du caractère à afficher.
 */
void putch(char data) {
    uartPutch(data);
}

/**
 * Implémentation d'une fonction système qui est appelée par <code>scanf</code>, 
 * <code>getc</code>, <code>getchar</code>, etc.
 * Cette implémentation récupère le caractère à la file de réception.
 * @param data Le code ASCII du récupéré.
 */
char getche() {
    char data = getch();
    putch(data);
    return data;
}

/**
 * Implémentation d'une fonction système qui est appelée par <code>scanf</code>, 
 * <code>getc</code>, <code>getchar</code>, etc.
 * Cette implémentation récupère le caractère à la file de réception.
 * @param data Le code ASCII du récupéré.
 */
char getch() {
    return uartGetch();
}
#endif


void initialiseUART1() {
    // Pour une fréquence de 8MHz, ceci donne 9600 bauds:
    SPBRG = 207;
    SPBRGH = 0;

    // Configure RC6 et RC7 comme entrées digitales, pour que
    // la EUSART puisse en prendre le contrôle:
    ANSELCbits.ANSC6 = 0;
    ANSELCbits.ANSC7 = 0;
    TRISCbits.RC6 = 1;
    TRISCbits.RC7 = 1;
    
    // Configure la EUSART:
    // (BRGH et BRG16 sont à leur valeurs par défaut)
    // (TX9 est à sa valeur par défaut)
    RCSTAbits.SPEN = 1;  // Active la EUSART.
    RCSTAbits.CREN = 1;  // Active le récepteur.
    TXSTAbits.SYNC = 0;  // Mode asynchrone.
    TXSTAbits.TXEN = 1;  // Active l'émetteur.
    BAUDCONbits.BRG16 = 1; //
    TXSTAbits.BRGH = 1;     //
    
}

// Initialisation de FC-114
void initialisationFC114(){
    
    TRISBbits.RB0 = 1;  // ENA du FC-114 en sortie
    PORTBbits.RB0 = 1;  // Module FC-114 actif
    
    
} 
