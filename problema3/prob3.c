#include <stdio.h>
#include <stdlib.h>

#define NOME_ARQUIVO_HDD "BACKING_STORE.bin"
#define NOME_ARQUIVO_ENDERECOS "addresses.txt"
//0000 0000 1111 1111
#define MASCARA_DESLOCAMENTO 255
//1111 1111 0000 0000
#define MASCARA_PAGINA 65280

#define TAMANHO_QUADRO 256

#define QUANTIDADE_QUADROS 256  

#define TAMANHO_TLB 16

FILE *hdd, *entrada;

//contadores
int acessos = 0;
int tlb_usada = 0;
int erros_pagina = 0;
int quadro_disponivel = 0;

int deslocamento = -1;
int pagina = -1;
int memoria_fisica[TAMANHO_QUADRO*QUANTIDADE_QUADROS];
int tabela_pagina[QUANTIDADE_QUADROS];

struct tlbData {
  unsigned int pagina;
  unsigned int quadro;
};

struct tlbData tlb[TAMANHO_TLB]; 


void pega_memoria_fisica(int endereco_logico){
    acessos++;
    deslocamento = endereco_logico & MASCARA_DESLOCAMENTO;
    pagina = (endereco_logico & MASCARA_PAGINA) >> 8;
    char buffer[TAMANHO_QUADRO];
    int endereco_fisico = -1;
    int valor = -1;

    int i;
    int quadro_achado = -1;

    for(i = 0; i<TAMANHO_TLB; i++){
        if(pagina == tlb[i].pagina){
            quadro_achado = tlb[i].quadro;
            tlb_usada++;
        }
    }


    if(quadro_achado != -1){
        valor = memoria_fisica[quadro_achado * TAMANHO_QUADRO + deslocamento];
    }else{
        if(tabela_pagina[pagina] != -1){
            quadro_achado = tabela_pagina[pagina];
            valor = memoria_fisica[quadro_achado * TAMANHO_QUADRO + deslocamento];
        }else{

            fseek(hdd,endereco_fisico, SEEK_SET);
            fread(buffer, sizeof(char),TAMANHO_QUADRO, hdd);
            
            quadro_achado = quadro_disponivel;

            for(i = 0; i<TAMANHO_QUADRO; i++){
                memoria_fisica[quadro_achado * TAMANHO_QUADRO + i] = atoi(buffer[i]);
            }
            valor = buffer[deslocamento];

            tabela_pagina[pagina] = quadro_achado;
            quadro_disponivel++;
            erros_pagina++;
            
            inserir_tlb(pagina, quadro_achado);

        }
    } 

    endereco_fisico = quadro_achado*TAMANHO_QUADRO + deslocamento;

    printf("Endereço logico: Endereço fisico: Quadro: Deslocamento: Valor ASCII:\n");
}


int main() {
    int i;
    for(i = 0; i < TAMANHO_QUADRO; i++){
        tabela_pagina[i] = -1;
    }
    
    hdd = fopen(NOME_ARQUIVO_HDD,"rb");
    entrada = fopen(NOME_ARQUIVO_ENDERECOS,"r");
 
    
    fclose(hdd);
    return 0;
}
