#include <stdio.h>
#include <stdlib.h>

#define NOME_ARQUIVO_HDD "BACKING_STORE.bin"
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
int tamanho_ocupado = 0;
int posicao_tlb = 0;

int deslocamento = -1;
int pagina = -1;
int memoria_fisica[TAMANHO_QUADRO*QUANTIDADE_QUADROS];
int tabela_pagina[QUANTIDADE_QUADROS];

struct tlbData {
  unsigned int pagina;
  unsigned int quadro;
};

struct tlbData tlb[TAMANHO_TLB]; 

void inserir_tlb(int pagina, int quadro){
      if(tamanho_ocupado == TAMANHO_TLB)
        tamanho_ocupado--;

      // FIFO
      for(posicao_tlb = tamanho_ocupado; posicao_tlb > 0; posicao_tlb--) {
        tlb[posicao_tlb].pagina = tlb[posicao_tlb-1].pagina;
        tlb[posicao_tlb].quadro = tlb[posicao_tlb-1].quadro;
      }

      if(tamanho_ocupado <= 15)
        tamanho_ocupado++;

      tlb[0].pagina = pagina;
      tlb[0].quadro = quadro;
}

void consulta_memoria_fisica(int endereco_logico){
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
                memoria_fisica[quadro_achado * TAMANHO_QUADRO + i] = buffer[i];
            }
            valor = buffer[deslocamento];

            tabela_pagina[pagina] = quadro_achado;
            quadro_disponivel++;
            erros_pagina++;
            
            inserir_tlb(pagina, quadro_achado);
        }
    } 

    endereco_fisico = quadro_achado*TAMANHO_QUADRO + deslocamento;

    printf("Endereco logico: %d Endereco fisico: %d Quadro: %d Deslocamento: %d Valor ASCII: %d\n", 
        endereco_logico, endereco_fisico, quadro_achado, deslocamento, valor);
}


int main(int argc, char *argv[]) {
    int i;
    int endereco_logico;   
    float taxa_erro_pagina, taxa_sucesso_TLB;

    if (argc != 2) {
        fprintf(stderr,"Use: ./a.out [input file].txt\n");
        return -1;
    }

    for(i = 0; i < TAMANHO_QUADRO; i++){
        tabela_pagina[i] = -1;
    }
    
    hdd = fopen(NOME_ARQUIVO_HDD, "rb");
    entrada = fopen(argv[1], "r");

    while(fscanf(entrada, "%d", &endereco_logico) == 1) {
        consulta_memoria_fisica(endereco_logico);
    }

    taxa_erro_pagina = ((float)erros_pagina/(float)acessos) * 100.0;
    taxa_sucesso_TLB = ((float)tlb_usada/(float)acessos) * 100.0;

    printf("\nEstatisticas:\n");
    printf("Numero de acessos: %d\n", acessos);
    printf("Quantidade de acertos TLB: %d\n", tlb_usada);
    printf("Quantidade de erros de pagina: %d\n", erros_pagina);
    printf("Taxa de erros de pagina: %.2f%%\n", taxa_erro_pagina);
    printf("Taxa de sucesso da TLB: %.2f%%\n", taxa_sucesso_TLB);
 
    fclose(hdd);
    fclose(entrada);

    return 0;
}
