#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

/* Structure representant un paquet*/
typedef struct
{
    char* nom_paquet;
}paquet;

/* Structure representant la file fifo*/
typedef struct {
    int tete;
    int queue;
    int taille;
    int nombreElements;
    paquet* file;
} fifo;

/* Fonction permettant de defiler la file*/
paquet defiler(fifo *f);

/* Fonction permettant d'enfiler un paquet*/
/* dans la file*/
int enfiler(fifo *f, paquet valeur);

/* Fonction indiquant si la file est pleine*/
bool estPlein(fifo *f);

/* Fonction indiquant si la file est vie*/
bool estVide(fifo *f);
