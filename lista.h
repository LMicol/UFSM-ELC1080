typedef struct no No;

void lst_cria(No ** inicio);
void lst_insere(No** inicio, int v);
void lst_retira(No** inicio);
void lst_imprime(No * inicio);
void lst_libera(No** inicio);
int  lst_info(No * inicio);
