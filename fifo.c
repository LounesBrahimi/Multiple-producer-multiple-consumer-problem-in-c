#include "fifo.h"
paquet defiler(fifo *f) {
    if (f->queue == f->tete) {
        perror("Pile vide");
        exit(EXIT_FAILURE);
    }
    paquet valeur = f->file[f->queue];
    f->queue++;
    f->nombreElements--;
    return valeur;
}

int enfiler(fifo *f, paquet valeur) {
    if (f->nombreElements >= f->taille) {
        return -1;
    }
    else if ((f->tete == f->queue) && (f->tete == f->taille)){
        f->tete = 0;
        f->queue = 0;
    }
    f->file[f->tete] = valeur;
    f->tete++;
    f-> nombreElements++;
    return 0;
}

bool estPlein(fifo *f){
    return (f->nombreElements >= f->taille);
}
bool estVide(fifo *f){
    return (f->nombreElements == 0);
}
