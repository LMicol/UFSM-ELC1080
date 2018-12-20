#include"disco.h"

/* Função que implementa o disco utilizando o fator de entrelaçamento
 * determinado pelos testes utilizando a função disco_entrelacamento
 * fator = 16.
*/
void disco_simulador(int id, int tipo, void* buf)
{
    /* id lógico -> <cilindro, setor, superficie> */
    int cilindro   = (id / NUM_SET_TRILHA) % NUM_TRILHAS;	/* Obtem o numero do cilindro */
    int setor      = id % NUM_SET_TRILHA;                   /* Obtem o numero do setor */
    int superficie = id / (NUM_TRILHAS * NUM_SET_TRILHA);	/* Obtem o numero da superficie */

    /* <cilindro, setor, superficie> -> id lógico */
	int novosetor = (setor + 16) % NUM_SET_TRILHA;	/* Novo setor deslocado N posições */
    int novoid = ((novosetor + cilindro * NUM_SET_TRILHA) % (NUM_TRILHAS * NUM_SET_TRILHA)) + superficie * (NUM_TRILHAS * NUM_SET_TRILHA);

	/* Chama a função que implementa o disco */
    disco_simulador_sem_entrelacamento(novoid,tipo,buf);
}

/* Função que implementa o disco sem entrelaçamento */
void disco_simulador_sem_entrelacamento(int id, int tipo, void* buf)
{
    FILE *arq;                /* Declara um ponteiro para arquivo */

    int tempo_espera = 0;

    /* Verifica se id é um setor valido, senão sai */
    if((id < 0) || (id >= NUM_SETORES))
    {
        exit(EXIT_FAILURE);
    }

    arq = abre_arquivo("disco.simulado","r+");            /* "r": Abre arquivo pra leitura */
	fseek(arq, id * TAM_SETOR, SEEK_SET);                /* Move a posição de leitura do inicio do arquivo para id * TAM_SETOR */

    /* Seleciona entre leitura e gravacao */
    if(tipo == LEITURA)
    {
        /* Testa se o setor foi lido */
        if(fread(buf, TAM_SETOR, 1, arq) != 1)
        {
            exit(EXIT_FAILURE);
        }
    }
    else if(tipo == GRAVACAO)
    {
        /* Testa se o setor foi escrito */
        if(fwrite(buf, TAM_SETOR, 1, arq) != 1)
        {
            exit(EXIT_FAILURE);
        }
    }

	fecha_arquivo(arq);    /* Fecha o arquivo apontado por arq */

    /* Calcula o tempo de espera em microsegundos */
    /* Incluído nesse tempo:
    * [1] tempo de busca pelo cilindro certo
    * [2] tempo de seleção do cabeçote (se não tiver esse tempo, considere 1/10 do tempo de busca de uma trilha)
    * [3] tempo de espera rotacional
    * [4] tempo de transferência do disco para a controladora
    * [5] tempo de transferência para a memória.
    * Para o tempo de espera rotacional, considere que o disco esteja sincronizado com o relógio do sistema: na hora que troca de segundo no relógio o cabeçote 0 da trilha 0 está sobre o intervalo logo antes do setor 0. */

    tempo_espera =+ (TMP_BUSCA_CIL_MEDIO * 1000);					/* [1] */

	/* Considera o tempo de troca do cabeçote quando id lógico estiver na segunda superficie */
	if(id >= NUM_TRILHAS * NUM_SET_TRILHA)
	{
		tempo_espera =+ TMP_BUSCA_CIL_MEDIO * 100;					/* [2] */
	}

	tempo_espera =+ ((60*1000000)/VEL_ROTACAO);						/* [3] */
	tempo_espera =+ ((60*1000000)/(NUM_SET_TRILHA * VEL_ROTACAO));	/* [4] */
	tempo_espera =+ (TAM_SETOR*1000000)/TAXA_TRANSFERENCIA;			/* [5] */

    /* Espera o tempo calculado */
    long t_init, t_end;
    t_init = wtime();
    t_end  = wtime();
    while((t_end - t_init) < tempo_espera)
    {
        t_end = wtime();
    }
}

/* Função que implementa entrelaçamento de setores numa trilha */
void disco_entrelacamento(int id, int tipo, void* buf, int fator)
{
    /* id lógico -> <cilindro, setor, superficie> */
    int cilindro   = (id / NUM_SET_TRILHA) % NUM_TRILHAS;	/* Obtem o numero do cilindro */
    int setor      = id % NUM_SET_TRILHA;                   /* Obtem o numero do setor */
    int superficie = id / (NUM_TRILHAS * NUM_SET_TRILHA);	/* Obtem o numero da superficie */

    /* <cilindro, setor, superficie> -> id lógico */
	int novosetor = (setor + fator) % NUM_SET_TRILHA;	/* Novo setor deslocado N posições */
    int novoid = ((novosetor + cilindro * NUM_SET_TRILHA) % (NUM_TRILHAS * NUM_SET_TRILHA)) + superficie * (NUM_TRILHAS * NUM_SET_TRILHA);

	/* Chama a função que implementa o disco */
    disco_simulador_sem_entrelacamento(novoid,tipo,buf);
}

/* Estrutura de dados utilizada em programacao paralela */
long wtime()
{
   struct timeval t;
   gettimeofday(&t, NULL);
   return t.tv_sec*1000000 + t.tv_usec;
}

/* Função para abertura de um arquivo  */
FILE* abre_arquivo(char* url, char* mode)
{
    FILE* arq;                /* Declara um ponteiro para arquivo */

    if(url == NULL)            /* Exit se nome do arquivo for NULL */
    {
        exit(EXIT_FAILURE);
    }

    arq = fopen(url,mode);    /* Abre o arquivo */

    if(arq == NULL)            /* Exit se arquivo não pode ser aberto */
    {
        exit(EXIT_FAILURE);
    }

    return arq;                /* Retorna ponteiro para o arquivo */
}

/* Função para fechamento de um arquivo  */
void fecha_arquivo(FILE* arq)
{
    if(arq != NULL)            /* Fecha o arquivo apontado por arq */
    {
        fclose(arq);
    }
}
