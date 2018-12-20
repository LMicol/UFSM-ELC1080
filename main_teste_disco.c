#include"disco.h"

int main()
{
    char* buf_dado  = (char*)malloc(TAM_SETOR * sizeof(char));

    long t_init_sg, t_end_sg,t_init_sl, t_end_sl;
	long t_init_ag, t_end_ag,t_init_al, t_end_al;
	int  qtd_setores = 0;
	int  fator = 0;
	int  i, j;
	FILE *arq;                /* Declara um ponteiro para arquivo */

	if((buf_dado != NULL)) /* Continua se o buf_dado foi alocado */
    {
		/* String de testes */
		strcpy(buf_dado,"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz0123456789");

		/* Escreve o cabecalho no arquivo */
		arq = abre_arquivo("resultados.csv", "w");
		fprintf(arq,"fator_entrelacamento,setores_gravados,tempo_gra_seq,tempo_lei_seq,tempo_grav_ale,tempo_lei_ale\n");
		fecha_arquivo(arq);

		/* Varia fator de entrelaçamento */
		for(fator = 0; fator < NUM_SET_TRILHA - 1; fator++)
		{
			/* Varia qtd de setores gravados entre 25, 50, 75 e 100% do total de setores do disco */
			for(qtd_setores = NUM_SETORES/4; qtd_setores <= NUM_SETORES; qtd_setores += NUM_SETORES/4)
			{
				/* Sequencial */
				/* Gravacao */
				t_init_sg = wtime();
				for(j = 1; j < qtd_setores; j++)
				{
					disco_entrelacamento(j,1,buf_dado,fator);
				}
				t_end_sg  = wtime();

				/* Leitura */
				t_init_sl = wtime();
				for(j = 0; j < qtd_setores; j++)
				{
					disco_entrelacamento(j,0,buf_dado,fator);
				}
				t_end_sl  = wtime();

 				/* Aleatorio */
				/* Gera um vetor de tamanho qtd_setores com números aleatorios menores que NUM_SETORES */
				int* setores_ale = (int*)malloc(qtd_setores * sizeof(int));
				if(setores_ale != NULL)
				{
					srand(time(NULL));
					for (i = 0; i < qtd_setores; i++)
					{
						setores_ale[i] = rand() % NUM_SETORES;
					}
				}
				else
				{
					free(buf_dado);
					return EXIT_FAILURE;
				}

				/* Gravacao */
				t_init_ag = wtime();
				for(j = 0; j < qtd_setores; j++)
				{
					disco_entrelacamento(setores_ale[j],1,buf_dado,fator);
				}
				t_end_ag  = wtime();

				/* Leitura */
				t_init_al = wtime();
				for(j = 0; j < qtd_setores; j++)
				{
					disco_entrelacamento(setores_ale[j],0,buf_dado,fator);
				}
				t_end_al  = wtime();

				free(setores_ale);

				/* Escreve os tempos em aquivo csv */
				arq = abre_arquivo("resultados.csv", "a");
				fprintf(arq,"%d,%d,%ld,%ld,%ld,%ld\n", fator,  qtd_setores, (long) (t_end_sg - t_init_sg), (long) (t_end_sl - t_init_sl), (long) (t_end_ag - t_init_ag), (long) (t_end_al - t_init_al));
				fecha_arquivo(arq);
			}
		}

		free(buf_dado);

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}
