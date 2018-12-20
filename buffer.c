#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#include "buffer.h"
#include "lista.h"

struct buff
{
	unsigned char* buffer;/* O Próprio buffer sem ser inicializado */
    No* buff_lista;       /* Lista dos tamanhos das insercoes */
	int buff_size;        /* Tamanho do buffer */
    int buff_pos_insere;  /* Marca a posicao atual no buffer */
    int buff_pos_remove;  /* Marca a posicao atual da remocao no buffer */
	int buff_ocupa;       /* Conta o numeros de bytes ocupados */
    sem_t lock;           /* Mutex */
	sem_t remove;         /* Semaforo que indica se tem algo pra remover */
};

buffer *buffer_inicializa(int cap)
{
    if(cap > 0) 
    {
        buffer* buff = (buffer*)malloc(sizeof(buffer));

        if(buff != NULL) 
        {   /* Continua se o buffer foi alocado, senao retorna NULL 
			   Aloca buff->buffer */
            buff->buffer = (unsigned char*)malloc(cap);

            if(buff->buffer != NULL) /* Continua se o buff->buffer foi alocado, senão retorna NULL */
            {
                lst_cria(&buff->buff_lista); /* Cria lista para os tamanhos dos dados inseridos no buffer */

				buff->buff_size = cap;
				buff->buff_ocupa = 0;
                buff->buff_pos_insere = 0;	
                buff->buff_pos_remove = 0;	

				sem_init(&buff->lock, 0, 1);
				sem_init(&buff->remove, 0, 0);	    
                
                return buff;
            }
            else
            {
                /* Libera buff antes de retornar NULL, se não foi possível alocar buff->buffer */
                free(buff);
            }
        }
    }
    /* Retorna NULL se algo deu errado */
    return NULL;
}

void buffer_finaliza(buffer *buf)
{
    /* Libera memória ocupada por um buffer e retorna */
    if(buf != NULL)
    {
        free(buf->buffer);				/* Libera memória alocada para o buffer */
        lst_libera(&buf->buff_lista);	/* Libera memória alocada para a lista de tamanhos dos dados */
        free(buf);						/* Libera memória alocada para o buffer */
    }

    return;
}

bool buffer_insere(buffer *buf, void *p, int tam)
{
    if(buf != NULL)
    {
        sem_wait(&buf->lock);
        char *valores = (char*)p;
        if((buf->buff_size - buf->buff_ocupa) < tam)
        {
            sem_post(&buf->lock);
            return false;
        }
        int i;
        /* Insercao modular */
        for(i = 0; i < tam; i++)
        {
            buf->buffer[((i + buf->buff_pos_insere) % buf->buff_size)] = valores[i];
        }

        buf->buff_pos_insere = ((buf->buff_pos_insere + tam) % buf->buff_size);
        buf->buff_ocupa += tam;
        
        lst_insere(&buf->buff_lista, tam);

        sem_post(&buf->remove);
        sem_post(&buf->lock);

        return true;
    }
    return false;
}


bool buffer_remove(buffer *buf, void *p, int cap, int *tam)
{
    if(!(buf == NULL || (p == NULL && cap != 0) || cap < 0 || tam == NULL))
    {    
        sem_wait(&buf->remove);        
        sem_wait(&buf->lock);

        char* aux = (char*)p;
        int quantidade = lst_info(buf->buff_lista);

        if(quantidade > 0)
        {
            /* Se cap for menor, j recebe cap, assim otimiza o laço for */
            int i, j;
            j = quantidade > cap ? cap : quantidade;
            /* Copia do buffer para o destino ate o limite da capacidade do destino */
            for(i = 0; i < j; i++)
            {
                aux[i] = buf->buffer[((i + buf->buff_pos_remove) % buf->buff_size)];
            }
            buf->buff_pos_remove = ((buf->buff_pos_remove + quantidade) % buf->buff_size);
        }
       
        lst_retira(&buf->buff_lista);
        sem_post(&buf->lock);
        *tam = quantidade;
        return true;
    }
    return false;
}