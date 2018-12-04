#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include "buffer.h"
#include "lista.h"

/* Contem todas as variáveis do buffer */
struct buff
{
	char* buffer;         /* O Próprio buffer sem ser inicializado */
    No* buff_lista;       /* Lista dos tamanhos das insercoes */
	int buff_size;        /* Tamanho do buffer */
    int buff_pos_insere;  /* Marca a posicao atual no buffer */
    int buff_pos_remove;  /* Marca a posicao atual da remocao no buffer */
	int buff_ocupa;       /* Conta o numeros de bytes ocupados */
    sem_t mutex;          /* Semaforo de exclusão mútua */
	sem_t remove;         /* Semaforo que indica se o buffer esta ocupado */
};

buffer *buffer_inicializa(int cap)
{
    if(cap > 0) /* Continua se a capacidade é positiva, senão retorna NULL */
    {
		/* Aloca buffer */
        buffer* buff = (buffer*)malloc(sizeof(buffer*));

        if(buff != NULL) /* Continua se o buffer foi alocado, senao retorna NULL */
        {
			/* Aloca buff->buffer */
            buff->buffer = (char*)malloc(cap);

            if(buff->buffer != NULL) /* Continua se o buff->buffer foi alocado, senão retorna NULL */
            {
                lst_cria(&buff->buff_lista); /* Cria lista para os tamanhos dos dados inseridos no buffer */

				buff->buff_size = cap;		/* Capacidade total de buff */
				buff->buff_ocupa = 0;		/* buff inicialmente esta vazio */
                buff->buff_pos_insere = 0;	/*  */
                buff->buff_pos_remove = 0;	/*  */

				sem_init(&buff->mutex, 0, 1);	/*  */
				sem_init(&buff->remove, 0, 0);	/*  */

                /* Retorna ponteiro do buffer se tudo ocorreu bem */
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
    sem_wait(&buf->mutex);	/* Inicia Regiao critica */

	/* Se não houver espaço para insercao, libera mutex e retorna falso */
    if((buf->buff_size - buf->buff_ocupa) < tam || tam < 0)
    {
        sem_post(&buf->mutex);
        return false;
    }

    char* to_buff = (char*)malloc(tam);
    if(to_buff != NULL)		/* Continua se o buffer foi alocado, senão retorna */
    {
        char *valores = (char*)p;
        int i;

        /* Insercao modular */
        for(i = 0; i < tam; i++)
        {
			/* Armazena no buffer */
            buf->buffer[((i + buf->buff_pos_insere) % buf->buff_size)] = valores[i];

			/* Atualiza posição de insercao */
            buf->buff_pos_insere = ((buf->buff_pos_insere + 1) % buf->buff_size);
        }

        buf->buff_ocupa += tam;				/* Atualiza contador de bytes ocupados */
        lst_insere(&buf->buff_lista, tam);	/* Insere tamanho do dado na lista */
        free(to_buff);    					/* Libera o espaço alocado previamente */
    }

    /* Fim da Regiao Critica */
    sem_post(&buf->mutex);
    sem_post(&buf->remove);
    return true;
}

/* alternativa para o buffer_remove
 * não seria necessário alocar 'dado' armazenando direto no buffer
 * até o limite da capacidade.
 * quando ultrapassar a capacidade, apenas atualiza a posição de remoção
*/
bool buffer_remove(buffer *buf, void *p, int cap, int *tam)
{
	sem_wait(&buf->remove);     /* Tenta remover, se nao espera */

    /* Teste de inconsistencia das entradas */
    if((p == NULL && cap != 0) || cap < 0)
	{
		return false;
	}

    sem_wait(&buf->mutex);      /* Entra na regiao critica */

    /* Busca a o tamanho do dado a ser retirado no buffer */
    int quantidade = lst_info(buf->buff_lista);

    if(tam)	/* Se tam nao eh valido, entradas tb nao sao consistentes. esse teste tb pode ser fora da regiao critica */
    {
		char* aux = (char*)p;
        int i;

		/* Remove os dados do buffer */
        for(i = 0; i < quantidade; i++)
        {
			/* Copia do buffer para o destino ate o limite da capacidade do destino */
			if(i < cap)
			{
				aux[i] = buf->buffer[((i + buf->buff_pos_remove) % buf->buff_size)];
			}
			/* Atualiza posição de insercao */
            buf->buff_pos_remove = ((buf->buff_pos_remove + 1) % buf->buff_size);
        }

        lst_retira(&buf->buff_lista); /* retira tamanho do dado da lista, essa operacao tem que estar sempre depois dos testes de consistencia, nunca antes */

        sem_post(&buf->mutex);   /* Termina regiao critica */

        flag_mutex = 1;          /* Salva na flag que saiu do mutex*/
		*tam = quantidade; /* Altera o valor referenciado por tam */

        return true;        /* RETURN FALSE SE TRUNCAR? */
    }

    sem_post(&buf->mutex);

    return false;
}

