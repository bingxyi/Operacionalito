#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSOS 100
#define MAX_LINHA 256
#define FATIA_CLOCK 3

typedef struct {
    int idProcesso;
    int tempoEntrada;
    int tempoIO;
    int tempoProcessamento;
    int prioridade;
} Processos;

// Função para inserir o processo na fila de prioridade
void inserirFilaPrioridade (Processos fila[], int *tamanho, Processos p) {
    int i = *tamanho - 1;

    // Desloca os processos de menor prioridade para trás
    while (i >= 0 && fila[i].prioridade > p.prioridade) {
        fila[i+1] = fila[i];
        i--;
    }
    fila [i+1] = p;
    (*tamanho)++;
}

// Função para remover o primeiro processo da fila
Processos removerFila(Processos fila[], int *tamanho) {
    Processos p = fila[0];
    for(int i = 1; i < *tamanho; i++) {
        fila[i - 1] = fila[i];
    }
    (*tamanho)--;
    return p;
}

// Fila simples para I/O
void inserirFilaIO(Processos fila[], int *tamanho, Processos p) {
    fila[*tamanho] = p;
    (*tamanho)++;
}

Processos removerFilaIO(Processos fila[], int *tamanho) {
    Processos p = fila[0];
    for(int i = 1; i < *tamanho; i++) {
        fila[i - 1] = fila[i];
    }
    (*tamanho)--;
    return p;
}

int main() {
    FILE *arquivo;
    char linha[MAX_LINHA];
    Processos todosProcessos[MAX_PROCESSOS];
    int numProcessos = 0;

    // Abrir o arquivo
    arquivo = fopen("processos.txt", "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Ler os processos do arquivo 
    while (fgets(linha, sizeof(linha), arquivo) != NULL && numProcessos < MAX_PROCESSOS) {
        linha[strcspn(linha, "\n")] = 0;    // encontrando o comprimento da subsequêcia inicial da string
        char *token = strtok(linha, ";"); // dividindo a string em tokens

        if(token != NULL) todosProcessos[numProcessos].idProcesso = atoi(token);
        token = strtok(NULL, ";");
        if(token != NULL) todosProcessos[numProcessos].tempoEntrada = atoi(token);
        token = strtok(NULL, ";");
        if(token != NULL) todosProcessos[numProcessos].tempoProcessamento = atoi(token);
        token = strtok(NULL, ";");
        if(token != NULL) todosProcessos[numProcessos].tempoIO = atoi(token);
        token = strtok(NULL, ";");
        if(token != NULL) todosProcessos[numProcessos].prioridade = atoi(token);
        
        numProcessos++;
    }
    fclose(arquivo);

    Processos filaProcessamento[MAX_PROCESSOS];
    Processos filaIO[MAX_PROCESSOS];
    int tamanhoFilaP = 0, tamanhoFilaIO = 0;
    int tempoAtual = 0;
    int finalizados = 0;

    // Arquivo de saída
    FILE *output = fopen("output.txt", "w");
    if (!output) {
        printf("Erro ao abrir o arquivo de saída.\n");
        return 1;
    }

    while(finalizados < numProcessos) {
        // Inserir processos que chegam neste ciclo
        for(int i = 0; i < numProcessos; i++) {
            if (todosProcessos[i].tempoEntrada == tempoAtual) {
                inserirFilaPrioridade(filaProcessamento, &tamanhoFilaP, todosProcessos[i]);
                printf("[Tempo %d] Processo %d entrou na fila de processamento (Prioridade %d)\n",
                tempoAtual, todosProcessos[i].idProcesso, todosProcessos[i].prioridade);
            }
        }

        // Processamento
        if (tamanhoFilaP > 0) {
            Processos p = removerFila(filaProcessamento, &tamanhoFilaP);
            int fatia = (p.tempoProcessamento < FATIA_CLOCK) ? p.tempoProcessamento : FATIA_CLOCK;
            p.tempoProcessamento -= fatia;
            tempoAtual += fatia;

            printf("[Tempo %d] Processo %d processado por %d ciclos (restante P:%d I/O:%d)\n",
            tempoAtual, p.idProcesso, fatia, p.tempoProcessamento, p.tempoIO);

            if(p.tempoIO > 0) {
                // Inserir na fila de I/O após o processamento
                inserirFilaIO(filaIO, &tamanhoFilaIO, p);

                printf("[Tempo %d] Processo %d movido para fila de I/O (Tempo I/O restante: %d)\n",
                tempoAtual, p.idProcesso, p.tempoIO);
            }
            else if (p.tempoProcessamento > 0) {
                // Volta para fila de processamento
                inserirFilaPrioridade(filaProcessamento, &tamanhoFilaP, p);

                printf("[Tempo %d] Processo %d retornou para fila de processamento (Tempo restante: %d)\n",
                tempoAtual, p.idProcesso, p.tempoProcessamento);                
            }
            else {
                // Processo finalizado
                fprintf(output, "%d;%d\n", tempoAtual, p.idProcesso);
                printf("[Tempo %d] Processo %d finalizado\n", tempoAtual, p.idProcesso);
                finalizados++;
            }
        }

        // I/O
        else if (tamanhoFilaIO > 0) {
            Processos p = removerFilaIO(filaIO, &tamanhoFilaIO);
            int fatiaIO = 2 * FATIA_CLOCK;
            int tempoIOReal = (p.tempoIO < fatiaIO) ? p.tempoIO : fatiaIO;
            p.tempoIO -= tempoIOReal;
            tempoAtual += tempoIOReal;

            printf("[Tempo %d] Processo %d  executou I/O por %d ciclos (restante P: %d I/O: %d)\n",
            tempoAtual, p.idProcesso, tempoIOReal, p.tempoProcessamento, p.tempoIO);

            if (p.tempoProcessamento > 0) {
                // Volta para a fila de processamento
                inserirFilaPrioridade(filaProcessamento, &tamanhoFilaP, p);

                printf("[Tempo %d] Processo %d movido para fila de processamento (Tempo processamento restante: %d)\n",
                tempoAtual, p.idProcesso, p.tempoProcessamento);

            }
            else if (p.tempoIO > 0) {
                // Volta para a fila de IO
                inserirFilaIO(filaIO, &tamanhoFilaIO, p);

                printf("[Tempo %d] Processo %d retornou para fila de I/O (Tempo I/O restante: %d)\n",
                tempoAtual, p.idProcesso, p.tempoIO);

            }
            else {
                // Processo finalizado (usando 1 ciclo)
                tempoAtual += 1;
                fprintf(output, "%d;%d\n", tempoAtual, p.idProcesso);
                printf("[Tempo %d] Processo %d finalizado após I/O\n", tempoAtual, p.idProcesso);                
            }
        }
    }
    fclose(output);
    printf("Simulação concluída. Resultados em 'output.txt'.\n");

    return 0;
}