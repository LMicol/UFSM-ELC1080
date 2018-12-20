#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

/* Dados sobre um floppy disk 3.5 de 1.44MB
 * Tamanho:
 * Velocidade de rotação:		300 RPM
 * Número de trilhas:			80
 * Setores por trilha:			18
 * Trilhas por cilindro:		2
 * Tamanho do setor:			512 bytes
 * Total de setores:			2880
 * Tempo de rotação:			200 ms
 * Tempo trasf de um setor:		11,11 ms
 * Tempo troca de trilha adj:	6 ms
 * Tempo troca de trilha medio:	77 ms
 * Taxa de transferencia:		500 Kbits/s ou 62500 bytes/s
 */

#define VEL_ROTACAO 300
#define NUM_TRILHAS 80
#define NUM_SET_TRILHA 18
#define NUM_TRI_CILINDRO 2
#define TMP_BUSCA_CIL_ADJ 6
#define TMP_BUSCA_CIL_MEDIO 77
#define TAM_SETOR 512
#define TAXA_TRANSFERENCIA 62500
#define NUM_SETORES (NUM_TRILHAS * NUM_SET_TRILHA * NUM_TRI_CILINDRO)

#define LEITURA 0
#define GRAVACAO 1

FILE* abre_arquivo(char* url, char* mode);
void fecha_arquivo(FILE *arq);
void disco_simulador(int id, int tipo, void* buf);
void disco_simulador_sem_entrelacamento(int id, int tipo, void* buf);
void disco_entrelacamento(int id, int tipo, void* buf, int fator);
long wtime();
