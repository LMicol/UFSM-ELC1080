#include <stdlib.h>
#include <stdio.h>

#include "lista.h"

struct no {
   int info;         // Guarda o valor da lista
   struct no* ant;   // Ponteiro para o próximo nó
   struct no* prox;  // Ponteiro para o nó anterior
};

void lst_cria(No ** inicio)
{  // Faz o nó inicial apontar para NULL
   *inicio = NULL;
}

void lst_insere (No** inicio, int v)
{  // Insere um novo nó no final da lista
   No * ultimo = *inicio;
   
   if (ultimo!=NULL){
      while (ultimo->prox!=NULL)
      {  // Procura o último nó da lista
         ultimo = ultimo->prox;
      }
   }

   No* novo = (No*) malloc(sizeof(No));
   if(novo == NULL){    // Cria um novo nó e confere se é válido
      exit(-7);         // Erro de alocação de lista
   }

   novo->info = v;
   novo->prox = NULL;
   novo->ant = ultimo;

   if (ultimo!=NULL)          // Se o último nó não é nulo
      ultimo->prox = novo;    // insere no ponteiro do último o proximo

   if (*inicio == NULL)       // Se o nó inicial é nulo
      *inicio  = novo;        // insere nó no inicio

}


// Remove elemento da lista
void lst_retira (No** inicio)
{
   No* p = *inicio;
   if (p == NULL)                // Nao tem o que retirar se inicio == null
      return ;

   No* proximo = p->prox;        // Pega o proximo no
   if (proximo == NULL)          // Se este for null, a lista fica vazia
      *inicio = NULL;            // Se nao, perde o primeiro elemento nela inserido
   else
      *inicio = proximo;

   free(p);                      // Libera o nó tirado da lista
}

void lst_imprime(No * inicio)
{  // Funcao de impressao para debug
   No * p = inicio;
   while (p!=NULL){
      printf("%d\n", p->info);
      p = p->prox;
   }
}

void lst_libera (No** inicio)
{  // Desaloca toda a Lista, começando pelo primeiro no
   No* p = *inicio;
   while (p != NULL) {
      No* t = p->prox;
      free(p);
      p = t;
   }
   *inicio = NULL;
}


int lst_info(No * inicio)
{  // Retorna o valor do nó
   return (inicio->info);
}
