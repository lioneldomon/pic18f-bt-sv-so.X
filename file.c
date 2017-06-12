#include <xc.h>
#include <stdio.h>
#include "file.h"
#include "uart.h"


typedef struct {
    /** Espace de mémoire pour stocker la file. */
    char file[FILE_TAILLE];

    /** Pointeur d'entrée de la file. */
    unsigned char fileEntree;

    /** Pointeur de sortie de la file. */
    unsigned char fileSortie;

    /** Indique si la file est vide. */
    char fileVide;

    /** Indique si la file est pleine. */
    char filePleine;
} File;

/**
 * Indique si la file est vide.
 */
char fileEstVide(File *file) {
    // Si la pointeur de sortie est égal u pointeur d'entrée et que le pointeur plein vaut 0, on renvoir 255 sinon 0
    return file->fileVide;
    //return ((file->fileSortie==file->fileEntree) && file->filePleine == 0) ? 255 : 0 ; 
}
/**
 * Indique si la file est pleine.
 */
char fileEstPleine(File *file) {
    
   return file->filePleine; 
   //return (file->filePleine == 255) ? 255 : 0; // Si le pointeur filePleine vaut 255 on renvoie 255 sinon 0
        
}
/**
 * Si il y a de la place dans la file, enfile un caractère.
 * @param c Le caractère.
 */
void fileEnfile(File *file, char c) {
    file->fileVide = 0;
    if (!file->filePleine) {
        file->file[file->fileEntree++] = c;
        if (file->fileEntree >= FILE_TAILLE) {
            file->fileEntree = 0;
        }
        if (file->fileEntree == file->fileSortie) {
            file->filePleine = 255;
        }
    }
}

/**
 * Si la file n'est pas vide, défile un caractère.
 * @return Le caractère défilé, ou 0 si la file est vide.
 */
char fileDefile(File *file) {
    char data;
    file->filePleine = 0;
    if (!file->fileVide) {
        data = file->file[file->fileSortie++];
        if (file->fileSortie >= FILE_TAILLE) {
            file->fileSortie = 0;
        }
        if (file->fileSortie == file->fileEntree) {
            file->fileVide = 255;
        }
        return data;
    }
    return 0;
}  



/**
 * Vide et réinitialise la file.
 */
void fileReinitialise(File *file) {
    
    file->fileEntree = 0; //On remet les pointeurs à 0: résultat la file est vide
    file->fileSortie = 0; //On remet les pointeurs à 0: résultat la file est vide
    file->filePleine = 0; //On remet les pointeurs à 0: résultat la file est vide
    file->fileVide = 255; //On remet les pointeurs à 255
}

/* Copie le contenu d'une file dans l'autre
 * File1 : source
 * File2 : destination
*/
void copieFile(File *file1, File *file2){
    char c = 0;
    while(c<FILE_TAILLE){
        file2->file[c]=file1->file[c];
        c++;
    }
}