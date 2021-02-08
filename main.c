#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include "fifo.h"

#define N_PRODUCTEURS   5 /*Nombre de threads producteurs*/
#define M_CONSOMMATEURS 4 /*Nombre de threads consommateurs*/
#define CIBLE_PRODUCTION 5
#define TAILLE_FILE CIBLE_PRODUCTION* N_PRODUCTEURS /* Taille de la file (tapis)*/

/*Booleen permettant d'indiquer si le compteur est à 0 ou pas*/
bool consommation = true;

/* Structure representant le tapis*/
/* fifo est la structure de donnees qui gere la liste des produits*/
typedef struct
{
    int capacite;
    fifo file_de_paquets;
}tapis;

/* Structure contenant de precieux outils utilises pour*/
/* gerer les threads, mutex, threads et semaphore*/
typedef struct
{
   pthread_t thread_producteur[N_PRODUCTEURS];
   pthread_t thread_consommateur[M_CONSOMMATEURS];
   pthread_mutex_t mutex;
   sem_t vide;
   sem_t plein;
}
gestion_concurrence;

/*Initialisation du mutex*/
static gestion_concurrence conccurence =
{
   .mutex = PTHREAD_MUTEX_INITIALIZER,
};

/* Initialisation de la capacite du tapis*/
static tapis t =
{
    .capacite = CIBLE_PRODUCTION* N_PRODUCTEURS,
};

/* Structure utilisee pour constituer les parametres de la fonction*/
/* du thread consommateur, compteur est un pointeur sur la variable*/
/* du meme nom, part est le nombre de produits que va consommer le thread*/
typedef struct  {
    int* compteur;
    int* part;
} args_consommateur;

/* Structure representant les arguments de la fonction du thread*/
/* consommateur*/
typedef struct  {
    int identifiant;
    args_consommateur* args;
} args_avec_id;

/* Fontion permettant de concatener deux strings*/
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/* Fonction identitaire du thread producteur*/
void *producteur(void *p_data)
{
    char* nom_produit = (char*) p_data;
    int i;
    for(i = 0; i < CIBLE_PRODUCTION; i++) {
        sem_wait(&conccurence.vide);
        pthread_mutex_lock(&conccurence.mutex);
        char* indice_string[3];
        paquet nv_paquet;
        sprintf(indice_string, "%d", i+1);
        nv_paquet.nom_paquet = concat(nom_produit, concat(" ", indice_string));
        printf("Production : %s\n", nv_paquet.nom_paquet);
        enfiler(&t.file_de_paquets, nv_paquet);
        pthread_mutex_unlock(&conccurence.mutex);
        sleep(1);
        sem_post(&conccurence.plein);
        }
}

/* Fonction identitaire du thread consommateur*/
void *consommateur(void* p_data)
{
    args_avec_id* arguments = malloc (sizeof(args_avec_id));
    arguments = p_data;
    int part =arguments->args->part[arguments->identifiant];
    int identifiant = arguments->identifiant;
    char* indice_string[3];
    sprintf(indice_string, "%d", identifiant);
    int i;
    for(i = 0; i < part; i++) {
        sem_wait(&conccurence.plein);
        pthread_mutex_lock(&conccurence.mutex);
        printf("%s\n",concat( concat( concat("C ", indice_string), " mange "), defiler(&t.file_de_paquets).nom_paquet));
        (*arguments->args->compteur)--;
        pthread_mutex_unlock(&conccurence.mutex);
        sleep(1);
        sem_post(&conccurence.vide);
    }
}

int main()
{
    int compteur = N_PRODUCTEURS * CIBLE_PRODUCTION;
    pthread_t producteurs[N_PRODUCTEURS];
    pthread_t consommateurs[M_CONSOMMATEURS];

    sem_init(&conccurence.vide,0,TAILLE_FILE);
    sem_init(&conccurence.plein,0,0);

    t.file_de_paquets.queue = 0;
    t.file_de_paquets.tete = 0;
    t.file_de_paquets.nombreElements = 0;
    t.file_de_paquets.taille = TAILLE_FILE;
    t.file_de_paquets.file = malloc(TAILLE_FILE*sizeof(paquet));

    char* nom_produits[] = { "Pomme", "Abricot", "Ananas", "Kiwi", "Cerise", "Prune"};

    int i;
    for(i = 0; i < N_PRODUCTEURS; i++) {
        pthread_create(&producteurs[i], NULL, (void *)producteur, (void*)nom_produits[i]);
    }

    args_avec_id* arguments[M_CONSOMMATEURS];

    for(i= 0; i<M_CONSOMMATEURS; i++){
        (arguments[i]) = malloc(sizeof(args_avec_id));
        arguments[i]->args = malloc(sizeof(args_consommateur));
        arguments[i]->args->compteur = &compteur;
        arguments[i]->args->part = (int*) malloc(M_CONSOMMATEURS*sizeof(int));
    }

    for (i=0; i <M_CONSOMMATEURS; i++)
        arguments[i]->args->part[i] = compteur / M_CONSOMMATEURS;

    for (i=0; i <compteur % M_CONSOMMATEURS; i++)
        arguments[i]->args->part[i]++;

    for(i = 0; i < M_CONSOMMATEURS; i++) {
        arguments[i]->identifiant = i;
        pthread_create(&consommateurs[i], NULL, (void *)consommateur, (void*)arguments[i]);
    }

    for(i = 0; i < N_PRODUCTEURS; i++) {
        pthread_join(producteurs[i], NULL);
    }

    for(i = 0; i < M_CONSOMMATEURS; i++) {
        pthread_join(consommateurs[i], NULL);
    }
    printf("\nCompteur : %d\n", compteur);

    pthread_mutex_destroy(&conccurence.mutex);
    sem_destroy(&conccurence.vide);
    sem_destroy(&conccurence.plein);

    free(t.file_de_paquets.file);
    for(i= 0; i<M_CONSOMMATEURS; i++){
        free(arguments[i]->args->part);
        free(arguments[i]->args);
    }
    return 0;
}
