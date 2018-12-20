#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "disco.h"

// Numero de threads para inserir
int G_insert;

//numero de thread para remover
int G_remove;

// Tamanho dos dados a serem inseridos no buffer
int G_data_s;


void* inserindo(void* b)
{   /* Thread para teste de inserção */
    //printf("-----%d %d %d-----\n", G_insert, G_remove, G_data_s);
    int v[G_data_s];
    int i;

    for(i = 0 ; i < G_data_s ; i++){
        v[i] = 7;
    }
    /*
    printf("\nEstou inserindo no  buffer: ");
    for(i = 0 ; i < 1 ; i++){
        printf("%d", v[i]);
    }
    printf("\n");
    */
    
    if(buffer_insere( (buffer*) b, (void*) v, G_data_s ))
    {
        //printf("Inseri\n");
    }
    else
    {
        //printf("\nDeu errado a inserção\n");
    }
}

void* removendo(void* b)
{   /* Thread para teste de remoção */    
    buffer *my_buffer = (buffer*)b;
    
    int p[G_data_s];
    int i;
    int j;

    bool r = buffer_remove(my_buffer, (void*)p, G_data_s, &i);

    if(r)
    {
        //printf("Removi\n");
        /*
        for(j = 0; j < i; j++){
            printf("%d", p[j]);   
        }*/
    }
    else
    {
        //printf("\nA remoção não funcionou\n");
    }
}


int main(int argc, char *argv[])
{
    srand(time(NULL));
    if(argc == 4)
    {
        G_insert = atoi(argv[1]);
        G_remove = atoi(argv[2]);
        G_data_s = atoi(argv[3]);

        //printf("%d %d %d", G_insert, G_remove, G_data_s);

        buffer* my_buffer;
        my_buffer = buffer_inicializa(737280);

        pthread_t insersoras [G_insert];
        pthread_t remosoras  [G_remove];

        int i;
        long inicio, fim;
        inicio = wtime();
        // Create Threads insere
        for(i = 0; i < G_insert; i++){
            if( pthread_create(&insersoras[i], NULL, (void*) inserindo, (void*) my_buffer ) ){
                printf("Error creating thread\n");
                return -1;
            }
        }
        // Create Threads remove
        for(i = 0; i < G_remove; i++){
            if( pthread_create(&remosoras[i], NULL, (void*) removendo, (void*) my_buffer ) ){
                printf("Error creating thread\n");
                return -2;
            }
        }
        
        // Join Threads insere
        for(i = 0; i < G_insert; i++){
            if( pthread_join(insersoras[i], NULL) ){
                printf("Error joing thread\n");
                return -3;
            }
        }
        // Join Threads remove
        for(i = 0; i < G_remove; i++){
            if( pthread_join(remosoras[i], NULL) ){
                printf("Error joing thread\n");
                return -4;
            }
        }
        /**/
        fim = wtime();
        // Tempo em microssegundos
        printf("%ld\n", ((long)(fim-inicio)));

        return 0;
    }
    return 1;
}