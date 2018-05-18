// gcc pthread.c -o pthread -lpthread

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 9
#define ELEMENTS 9
#define ARQUIVO "sudokus/1.txt"

struct QUADRANTE
{
    int id;
};

void *coluna();
void *linha();
void *grid(void *param);

int sudoku[9][9];
int sudoku_coluna = 1;
int sudoku_linha = 1;

int main()
{
    FILE *arquivo;   
    arquivo = fopen(ARQUIVO, "r");
     
    pthread_t tid_quadrante[NUM_THREADS];
    pthread_attr_t attrs[NUM_THREADS];

    pthread_t tid_linha;
    pthread_t tid_coluna;

    pthread_attr_t attr_linha;
    pthread_attr_t attr_coluna;

    pthread_attr_init(&attr_coluna);
    pthread_attr_init(&attr_linha);
    
    if(arquivo == NULL){
        printf("Arquivo nao encontrado.\n");
        system("pause");
        return -1;
    }
    
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            if(fscanf(arquivo, "%d ", &sudoku[i][j])){
                continue;   
            }else{
                if(fscanf(arquivo, "%d\n", &sudoku[i][j]) || fscanf(arquivo, "%dEOF", &sudoku[i][j])){
                    continue;
                }else{
                    printf("Arquivo com formato incorreto.\n");
                    return -1;
                }
            }
        }
    }

    pthread_create(&tid_coluna, &attr_coluna, coluna, NULL);
    pthread_create(&tid_linha, &attr_linha, linha, NULL);

    struct QUADRANTE args[NUM_THREADS];
    
    for(int i = 0; i < NUM_THREADS; i++)
    {
        args[i].id = i;
        pthread_attr_init(&attrs[i]);
        pthread_create(&tid_quadrante[i], &attrs[i], grid, &args[i]);
    }

    pthread_join(tid_linha, NULL);
    pthread_join(tid_coluna, NULL);

    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(tid_quadrante[i], NULL);
    }

    if(sudoku_linha && sudoku_coluna){
        printf("Sudoku Valido\n");
    } else {
        printf("Sudoku Invalido\n");
    }

    return 0;
}

void *grid(void *param)
{
    struct QUADRANTE *arg = (struct QUADRANTE *)param;
    int id = arg->id;

    pthread_exit(0);
}

void *linha()
{
    int i, j, k;
    int numero_repetido = 0;

    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 9; j++)
        {
            for(k = j + 1; k < 9; k++)
            {
                if(sudoku[i][j] == sudoku[i][k])
                {
                    numero_repetido++;
                    break;
                }
            }
        }
    }

    if(numero_repetido != 0){
        sudoku_linha = 0;
    }

    printf("Fim da thread linha\n");

    pthread_exit(0);
}

void *coluna()
{
    int i, j, k;
    int numero_repetido = 0;

    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 9; j++)
        {
            for(k = j + 1; k < 9; k++)
            {
                if(sudoku[j][i] == sudoku[k][i])
                {
                    numero_repetido++;
                    break;
                }
            }
        }
    }

    if(numero_repetido != 0){
        sudoku_coluna = 0;
    }

    printf("Fim da thread coluna\n");

    pthread_exit(0);
}