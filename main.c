#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "disco.h"
#include "buffer.h"

// Total de clientes
#define T_CLIENTES 20
// Tamanho dos pacotes
#define T_PACOTES 512

buffer *server_buffer;
buffer *client_buffer[T_CLIENTES];

sem_t sem_resposta;

void request(int setor, int op, int id_buffer, char* gravar)
{
    char pedido[TAM_SETOR   ];
    char set[5], o[3], id[4];
    sprintf(set, "%d-", setor);
    sprintf(o, "%d-", op);
    sprintf(id, "%d-", id_buffer);

    // 0 = leitura
    if(op == 0)
    {
        sprintf(pedido, "%s%s%s%s", set, o, id, "NULL");
        //printf("Estou inserindo no servidor  th = %d\n", id_buffer);
        buffer_insere(server_buffer, (void*)pedido, T_PACOTES);
    }
    // 1 = Gravacao
    else if(op == 1)
    {
        sprintf(pedido, "%s%s%s%s", set, o, id, gravar);
        //printf("Estou inserindo no servidor  th = %d\n", id_buffer);
        buffer_insere(server_buffer, (void*)pedido, T_PACOTES);
    }
    else
    {
        pthread_exit(NULL);
    }
}

int response()
{   
    int t;
    char dado [TAM_SETOR];
    char *token;
    int setor, op, id;
    //printf("thread SERVIDOR = %d resposta\n", 1);
    if(buffer_remove(server_buffer, (void*)dado,  TAM_SETOR, &t) == true)
    {   // faz uma remoção e separa a partir do traço colocado na inserção
        //printf("thread SERVIDOR = %d resposta removendo\n", 1);
        const char dash[2] = "-";
        token = strtok(dado, dash);
        setor = atoi(token);
        token = strtok(NULL, dash);
        op = atoi(token);
        token = strtok(NULL, dash);
        id = atoi(token);
        token = strtok(NULL, "");


        if(op == 0){
            // Se for leitura
            char info[512];
            disco_simulador(setor, op, &info);
            buffer_insere(client_buffer[id], (void*)info, sizeof(info));
            return id;
        }else{
            // Se for gravação
            disco_simulador(setor, op, token);
            char ok[] = "OK";
            buffer_insere(client_buffer[id], (void*)ok, sizeof(ok));
            return id;
        }
    }
    return -1;
}


void *server()
{
    while(true)
    {
        sem_wait(&sem_resposta);
        int i = response();
        printf("thread SERVIDOR respondeu Thread = %d\n", i);
    }

	pthread_exit(NULL);
}

void *client(void *id)
{
    int a = (int)(intptr_t)id;

    // Executa requisições de tempo em tempo
    while(true)
    {      
        // setor aleatório
        int setor = rand()%NUM_SETORES;
        // operação aleatoria
        int opera = rand()%2;
        if(opera == 0){
            printf("thread CLIENTE = %d requisitando uma LEITURA\n", a);
            //printf("thread cliente = %d efetuando leitura\n", a);
            // se leitura, efetua o request
            request(setor, opera, a, NULL);
        }else{
            printf("thread CLIENTE = %d requisitando uma GRAVAÇÃO\n", a);
            //printf("thread cliente = %d efetuando gravação\n", a);
            // se gravarção, gera um request para simulação
            char c[] = "It'sforyouBluebatterednavaltownslipkissesdeliveredbyduckMusclesandbottlenosedgriftersarriveintimetocatchthelateshowIt'sabeehivebarrelraceAshehivestareandchasewastedfeaturewhotriedandfailedtoreachherEmbossedbeneathaboxintheclosetthat'slostThekindthatyoufindwhenyoumindyourownbusinessShivsistertothequicknessbeforeitblistersintothenewmorningmilkblanketYourilkisfunnytotheturnstyletouchbunnywho'sbouquetsetacourseforbloomwithoutdecayGetyoubroomandsweeptheechoesofyesternightsfallenfreckles,away";
            // e efetua o request
            request(setor, opera, a, c);
        }
        sem_post(&sem_resposta);
        // Apenas para podermos simular requisições
        sleep(rand()%2 + 2);
    }

	pthread_exit(NULL);
}



int main()
{
	pthread_t servidor, clientes[T_CLIENTES];	
    sem_init(&sem_resposta, 0 , 0);

    int i;

    // Cria e inicializa o buffer do servidor com suporte para 5000 pedidos
    server_buffer = buffer_inicializa(T_PACOTES*5000);
    for(i = 0; i < T_CLIENTES; i ++){
        client_buffer[i] = buffer_inicializa(T_PACOTES*5000);
    }

    if( pthread_create(&servidor, NULL, server, NULL) ){
        printf("Error creating thread\n");
        return -1;
    }
    for(i = 0; i < T_CLIENTES; i++){
        if( pthread_create(&clientes[i], NULL, client, (void *)(intptr_t) i) ){
            printf("Error creating thread\n");
            return -1;
        }
    }   

    if( pthread_join(servidor, NULL)){
        printf("Error creating thread\n");
        return -1;
    }
    for(i = 0; i < T_CLIENTES; i++){
        if( pthread_join(clientes[i], NULL) ){
            printf("Error creating thread\n");
            return -1;
        }
    }   
}

