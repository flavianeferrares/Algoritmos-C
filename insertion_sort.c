#include <stdio.h>
#include <stdlib.h>
#define MAX 10

void preenche_vetor(int *v);
void insertionSort(int *v);
void exibir_vetor(int *v);

int main(){

 int vetor[MAX];
 preenche_vetor(vetor);   
 insertionSort(vetor);
 exibir_vetor(vetor);
    
}

void preenche_vetor(int *v){
    int i, valor;

    for(i = 0; i < MAX; i++){
        printf("\nInforme um valor: ");
        scanf("%d", &v[i]);
    }
}

void insertionSort(int *v){
    int i, j;
    for(i = 1; i < MAX; i++){
        int valor = v[i];
        for(j = i-1; j >= 0 && v[j] > valor; j--){
            v[j + 1] = v[j];
        }
        v[j + 1] = valor;
    }
}

void exibir_vetor(int *v){
    int i;
    for(i = 0; i < MAX; i++){
        printf("\n %d", v[i]);
    }
}