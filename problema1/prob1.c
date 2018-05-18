// gcc prob1.c -o prob1.out -lpthread

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

const int INDEX[9][2] = {
                        {0, 0}, 
                        {0, 1},
                        {0, 2},
                        {1, 0},
                        {1, 1},
                        {1, 2},
                        {2, 0},
                        {2, 1},
                        {2, 2}};
int sudoku[9][9];
int sudoku_coluna = 1;
int sudoku_linha = 1;
int sudoku_grid = 1;

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

    if(sudoku_linha && sudoku_coluna && sudoku_grid){
        printf("Sudoku Valido\n");
    } else {
        printf("Sudoku Invalido\n");
    }

    return 0;
}

void *grid(void *param)
{
    struct QUADRANTE *arg = (struct QUADRANTE *)param;
    int numero_quadrante = arg->id;
    int i, j, k, numero_repetido = 0;

    int linha_inicial = INDEX[numero_quadrante][0]*3;
    int coluna_inicial = INDEX[numero_quadrante][1]*3;
    int grid[9];

    //guardando valores do quadrante em um array
    for(i = 0; i < 3; i++){
        for(j = 0; j < 3; j++){
            grid[i*3+j] = sudoku[i+linha_inicial][j+coluna_inicial];
        }
    }

    for(j = 0; j < 9; j++)
    {
        for(k = j + 1; k < 9; k++)
        {
            if(grid[j] == grid[k])
            {
                numero_repetido++;
                break;
            }
        }
    }

    if(numero_repetido != 0){
        sudoku_grid = 0;
    }

    printf("Fim da thread grid de id: %d\n", numero_quadrante);

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