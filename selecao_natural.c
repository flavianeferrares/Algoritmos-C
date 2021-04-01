#include <stdlib.h>
#include <stdio.h>
#include "auxiliar.h"

#define SENTINELA 2147483647
/*
    Aluno: Flaviane Vitoria Cruz Ferrares   18.1.8147 
    Para compilar pelo cmd -> gcc -o <nome_executavel> selecao_natural.c auxiliar.h
    .\nome_executavel
*/


void print_tela(FILE* arq)
{
    rewind(arq);    //descola para a posição inicial
    int num;

    //percorre o arquivo pegando o valor em binario e exibe em tela
    while(1)
    {
        fread(&num, sizeof(int), 1, arq);
        if(feof(arq)) break;
        printf("%d ", num);
    }
    printf("\n");   
}

//Função para verificar se o reservatorio esta cheio ou arquivo chegou ao fim  e escrever o conteúdo na partição
void escreve_particao_memoria(Registro *memoria, FILE *particao, int max){

    Indice_registro indice;
    int i;
    for(i = 0; i < max - 1; i++)
    {
      // a cada iteração pega o menor e escreve na partição atual
      indice = menor_particao(memoria, max);
      memoria[indice.i].key = SENTINELA; 
      fwrite(&indice.r.key, sizeof(int), 1, particao);
    }
}

//Função que se receber o último parâmetro 1, retorna o número maximo de partições.
//Se receber o último parâmentro 0, retorna a quantidade de registros no arquivo.
int numero_particoes(FILE *f, int max, int decisao){
    int a, cont = 0;
    while(feof(f) == 0)
    {
        fread(&a, sizeof(int), 1, f);
        cont++;
    }
    if(decisao) return (cont/(max * 2,72)) + 1;
    return cont-1;
}

void set_registro(Registro *r, int max){
    //seta o reservatorio com -1 para obter controle sobre o reservatorio
    int i;
    for(i = 0; i < max; ++i) r[i].key = -1;
}

//Alocar um registro
Registro* alloca_registro(){
    Registro *aux = (Registro*)malloc(sizeof(Registro));
    return aux;
}

//Função retorna a menor chave da memoria e o indice correspondente a ela
Indice_registro menor_particao(Registro *r, int max)
{
    Registro aux;
    Indice_registro indice;
    int menor;

    menor = r[0].key;
    indice.i = 0;
    int i;
    for (i = 1; i < max; i++)
    {
        if(!compare_menor(menor, r[i].key)){
            menor = r[i].key;
            indice.i = i;
        }
    }
    indice.r.key = menor;
    return indice;
}

//retorna 1 se a < b e 0 caso contrário
int compare_menor(int a, int b)
{
    if(a < b) return 1;
    return 0;
}

int selecao_natural(int max, FILE *file)
{
    int carrega1, controle_arquivo, len_num_arquivo, cont_reservatorio, controle_registros_lidos, max_registros;
    char nome_arquivo[numero_particoes(file, max, 1) + 5], *num;
    FILE *particao;
    Indice_registro ind_r;
    Registro *memoria, *reservatorio, *aux;

    rewind(file);
    carrega1 = 1;           //primeira vez entra no arquivo e são lidos n registros de uma vez
    controle_arquivo = 1;    //saber numero de partição atual
    len_num_arquivo = 0;     //quantos caracteres tem o numero da versao atual
    cont_reservatorio = 0;   // o indice atual do reservatório
    controle_registros_lidos = max - 1; //quantos registros foram lidos
    max_registros = numero_particoes(file, max, 0);  //numero total de registros

    rewind(file);
    memoria = (Registro*)malloc(sizeof(Registro)*max);
    reservatorio = (Registro*)malloc(sizeof(Registro)*max);

    aux = alloca_registro();

    sprintf(nome_arquivo, "part_%d", controle_arquivo); //colando o numero da partição no nome do arquivo

    set_registro(memoria, max);         //setando a memória
    set_registro(reservatorio, max);    //setando o reservatorio

    while(1){
        controle_registros_lidos++;
        
        if(carrega1)
        {    
            int i;
            for(i = 0; i < max; i++)
            {
                fread(&memoria[i].key, sizeof(int), 1, file); //Joga os N registros que cabem na memória para a memória
            
            }

            //encontra o menor, abre a primeira partição e escreve o menor na partição
            ind_r = menor_particao(memoria, max);
            particao = fopen(nome_arquivo, "wb");
            fwrite(&ind_r.r.key, sizeof(int), 1, particao);

            carrega1 = 0; //para não entrar mais nesse bloco condicional
            continue; 
        }

        fread(&aux->key, sizeof(int), 1, file); //lê o próximo elemento do arquivo

        /*Se o elemento lido for menor que o último elemento gravado na partição vai para o reservatório
        e o indice do reservatório é incrementado*/
        if(ind_r.r.key > aux->key)
        {
            reservatorio[cont_reservatorio].key = aux->key;
            cont_reservatorio++;
        }
        
        /*Se o último elemento lido for maior que o último elemento gravado na partição
        ele sera colocado na memória no lugar do último elemento(no seu respectivo indice)*/
        else
        {
            memoria[ind_r.i].key = aux->key;    //ind_r.i indice do ultimo menor elemento
            ind_r = menor_particao(memoria, max);
            fwrite(&ind_r.r.key, sizeof(int), 1, particao);

        }

        /*Se o reservatório estiver cheio ou o último registro lido*/
        if(cont_reservatorio == max || controle_registros_lidos == max_registros)
        {
            memoria[ind_r.i].key = SENTINELA;                   //o ultimo menor vira sentinela para não ser escolhido de novo
            escreve_particao_memoria(memoria, particao, max);   //escreve o que sobrou na menoria na particao atual
            fclose(particao);                                   //fecha o arquivo, sera criada uma nova partição

            controle_arquivo++; //número da nova partição

            if(cont_reservatorio == 0) break;  // reservatorio vazio, encerra o programa

            sprintf(nome_arquivo, "part_%d", controle_arquivo);   //nova partição
            particao = fopen(nome_arquivo, "wb");
            
            int i;
            for(i = 0; i < cont_reservatorio; i++)
            {
                memoria[i].key = reservatorio[i].key; //coloca na memoria os dados do reservatorio
            }

            /*Caso todos os registros estiverem sido lidos e o reservatório não estiver vazio
            cria-se uma nova partição e é colocado os dados presentes no reservatório nela*/
            if(controle_registros_lidos == max_registros && cont_reservatorio != 0)
            {
                sprintf(nome_arquivo, "part_%d", controle_arquivo);
                particao = fopen(nome_arquivo, "wb");
                escreve_particao_memoria(memoria, particao, cont_reservatorio + 1);
                fclose(particao);
                controle_arquivo++;
                break;
            }

            set_registro(reservatorio, max);
            ind_r = menor_particao(memoria, max);

            fwrite(&ind_r.r.key, sizeof(int), 1, particao);
            cont_reservatorio = 0; //seta o indice do reservatorio

        }
        
        if(controle_registros_lidos > max_registros)break;
    }

    /*Imprimir no terminal as partições obtidas*/
    int i;
    for (i = 1; i < controle_arquivo; i++) {
        sprintf(nome_arquivo, "part_%d", i);
        printf("%s : \n", nome_arquivo);
        particao = fopen(nome_arquivo, "rb");
        print_tela(particao);
    }

    return 1;
}

int main()
{
    int max;
    char arq_aux[20];
    FILE *arquivo;

    fprintf(stderr, "\nInforme o numero maximo de registros a serem carregados na memoria por vez: \n");
    scanf("%d", &max);

    fprintf(stderr, "\nDigite o nome do arquivo ou o path caso esteja em uma pasta diferente: \n");
    scanf("%s", arq_aux);

    arquivo = fopen(arq_aux, "rb");

    if(selecao_natural(max, arquivo)) fprintf(stderr, "\nClassificacao feita com sucesso\n");
    else fprintf(stderr, "\nErro inesperado\n");

}