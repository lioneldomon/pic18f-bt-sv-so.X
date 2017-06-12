#ifndef FILE_H
#define	FILE_H


#define FILE_TAILLE 100

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

void fileEnfile(File *, char c);
char fileDefile(File *);
char fileEstVide(File *);
char fileEstPleine(File *);
void fileReinitialise(File *);
void copieFile(File *, File *);

#ifdef TEST
int testFile();
#endif

#endif
