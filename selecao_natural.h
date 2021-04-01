#include <stdio.h>
#include <stdlib.h>

typedef struct registros {
  int key;
} Registro;

typedef struct pega_posicao_registro {
  Registro r;
  int i;
} Indice_registro;

int selecao_natural(int max, FILE *file);
void set_registro(Registro *r, int max);
Indice_registro menor_particao(Registro *r, int max);
int compare_menor(int a, int b);