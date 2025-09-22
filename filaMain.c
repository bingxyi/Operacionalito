#include <stdio.h>
#include <stdlib.h>

// #define MAX 15      
#define QUANTUM_CPU 3  // Fatia de tempo de clock para processamento
#define QUANTUM_IO 6 // Fatia de tempo de clock para I/O

struct Processo {
    int id;                 // Identificador do processo
    int tempoEntrada;       // Ciclo em que o processo entra no sistema
    int tempoIO_total;      // Tempo total necessário em I/O
    int tempoProc_total;    // Tempo total necessário em CPU
    int prio;               // Prioridade (1 = mais alta, 5 = mais baixa)
    int tempoIO_restante;   // Tempo de I/O restante
    int tempoProc_restante; // Tempo de processamento restante
    int tempoSaida;         // Ciclo em que o processo foi concluído
    struct Processo* proximo; // Ponteiro para próximo processo (lista encadeada)
};


/**
Inserir um processo na fila de forma ordenada pela prioridade
Quanto menor o valor de prio, mais a frente ele sera inserido na fila
cabecaFila e a cabeca atual da fila, ou seja, o topo
novoProcesso e o novo processo a ser inserido
**/
struct Processo* inserirEmFila (struct Processo* cabecaFila, struct Processo* novoProcesso) {
    // Caso 1: A fila esta vazia ou o novo processo tem maior prioridade que o primeiro item da fila
    if (cabecaFila == NULL || novoProcesso->prio < cabecaFila->prio) {
        novoProcesso->proximo = cabecaFila;
        return novoProcesso;
    }

    // Caso 2: Percorre a fila para encontrar o local de insercao
    struct Processo* atual = cabecaFila;

    // Procura a posicao correta da fila
    while (atual->proximo != NULL && novoProcesso->prio >= atual->proximo->prio) {
        atual = atual->proximo;
    }

    // Inserir o novo processo na posicao encontrada
    novoProcesso->proximo = atual->proximo;
    atual->proximo = novoProcesso;

    return cabecaFila;
}

/**
Remove e retorna o processo com maior prioridade para a cabeca da fila
ponteiro duplo para modificar a cabeca da fila na sua origem
o processo removido ou NULL se a fila estiver vazia
**/
struct Processo* removerDaFila (struct Processo** cabecaFila) {
    // Caso a fila esteja vazia
    if (cabecaFila == NULL || *cabecaFila == NULL) {
        return NULL;
    }

    struct Processo* processoRemovido = *cabecaFila;
    *cabecaFila = processoRemovido->proximo;

    processoRemovido->proximo = NULL;
    return processoRemovido;
}

/**
Insere um processo no final de uma fila (usado para a fila de I/O, que é FIFO).
Esta função necessária para a lógica de I/O sem prioridade.
**/
void inserirFimFila(struct Processo** cabecaFila, struct Processo* novoProcesso) {
    novoProcesso->proximo = NULL;
    if (*cabecaFila == NULL) {
        *cabecaFila = novoProcesso;
        return;
    }
    struct Processo* atual = *cabecaFila;
    while (atual->proximo != NULL) {
        atual = atual->proximo;
    }
    atual->proximo = novoProcesso;
}

/**
Função para leitura do arquivo
Carrega os processos do arquivo processos.txt para uma lista
**/
struct Processo* readFile (const char* processos) {
    FILE *file = fopen("processos.txt", "r");
    if (file == NULL) {
        printf("ERROR: Nao foi possivel abrir o arquivo\n");
        return NULL;
    }

    struct Processo* cabecaLista = NULL;

    int id, tempoEntrada, tempoIO, tempoProc, prio;
    int contador = 0;

    // Alocando memoria para o novo processo
    while (fscanf(file, "%d;%d;%d;%d;%d", &id, &tempoEntrada, &tempoIO, &tempoProc, &prio) == 5) {
        struct Processo* novoProcesso = (struct Processo*)malloc(sizeof(struct Processo));
        if (novoProcesso == NULL) {
            printf("Nao foi possivel alocar memoria para o processo\n");
            fclose(file);
            return NULL;
        }
        novoProcesso->id = id;
        novoProcesso->tempoEntrada = tempoEntrada;
        novoProcesso->tempoIO_total = tempoIO;
        novoProcesso->tempoProc_total = tempoProc;
        novoProcesso->prio = prio;

        novoProcesso->tempoIO_restante = tempoIO;
        novoProcesso->tempoProc_restante = tempoProc;
        novoProcesso->tempoSaida = 0;

        inserirFimFila(&cabecaLista, novoProcesso);
        contador++;
    }

    fclose(file);
    printf("Arquivo lido com sucesso. %d processos carregados.\n\n", contador);
    return cabecaLista;
}

/**
Escreve o resultado do sistema em um arquivo "saida.txt"
**/
void escreverArquivoSaida(struct Processo* processosFinalizados) {
    FILE *file = fopen("saida.txt", "w");
    if (file == NULL) {
        printf("Nao foi possivel criar o arquivo de saida\n");
        return;
    }
    fprintf(file,"Tempo de saida;ID do processo\n");
    struct Processo* atual = processosFinalizados;

    while (atual != NULL) {
        fprintf(file, "%d;%d\n", atual->tempoSaida, atual->id);
        atual = atual->proximo;
    }
    fclose(file);
    printf("\nArquivo saida.txt criado com sucesso\n");
}

// Boa prática para evitar travamentos ou lixo na saida do codigo
// Libera toda a memória alocada para uma lista de processos.
void liberarMemoria(struct Processo* cabecaFila) {
    struct Processo* atual = cabecaFila;
    struct Processo* proximo = NULL;

    while (atual != NULL) {
        proximo = atual->proximo;   // 1. Guarda a referência para o proximo nó
        free(atual);                // 2. Libera a memória do nó atual
        atual = proximo;            // 3. Avança para o próximo nó que guardamos
    }
}

// Função principal do sistema
int main() {
    // Listas e filas do Sistema
    struct Processo* listaDeEspera = readFile("processos.txt");
    struct Processo* filaDeProcessamento = NULL;
    struct Processo* filaDeIO = NULL;
    struct Processo* processosFinalizados = NULL;

    if (listaDeEspera == NULL) {
        return 1;   // Encerra o programa caso não consiga ler o arquivo
    }

    int cicloAtual = 0;
    int totalProcessos = 15;
    int processosConcluidos = 0;

    // Loop principal: continua enquanto houver processos em qualquer lugar do sistema
    while (processosConcluidos < totalProcessos) {
        // 1. Novos Processos
        while (listaDeEspera != NULL && listaDeEspera->tempoEntrada <= cicloAtual) {
            struct Processo* p = removerDaFila(&listaDeEspera);
            printf("Ciclo %-3d: [ENTRADA] Processo %2d (Prio %d) entrou no sistema.\n", cicloAtual, p->id, p->prio);
            filaDeProcessamento = inserirEmFila(filaDeProcessamento, p);
        }

        // 2. Processamento de I/O
        if (filaDeIO != NULL) {
            struct Processo* p = removerDaFila(&filaDeIO);
            p->tempoIO_restante -= QUANTUM_IO;

            if (p->tempoIO_restante <= 0) {
                printf("Ciclo %-3d: [I/O FIM] Processo %2d terminou I/O. ", cicloAtual, p->id);
                // Se não precisa mais de CPU, vai para o ciclo final de encerramento
                if (p->tempoProc_restante <= 0) {
                     p->tempoProc_restante = 1;
                     printf("Indo para o ciclo de encerramento.\n");
                } else {
                     printf("Voltando para a fila de CPU.\n");
                }
                filaDeProcessamento = inserirEmFila(filaDeProcessamento, p);
            } else {
                // Se ainda precisa de I/O, volta para a fila de I/O
                inserirFimFila(&filaDeIO, p);
            }
        }

        // 3. Processamento da CPU
        if (filaDeProcessamento != NULL) {
            struct Processo* p = removerDaFila(&filaDeProcessamento);

            // Caso especial de encerramento (1 ciclo)
            if (p->tempoProc_restante == 1 && p->tempoIO_restante <= 0 && p->tempoProc_total == 0) {
                p->tempoSaida = cicloAtual + 1;
                printf("Ciclo %-3d: [CONCLUÍDO] Processo %2d finalizado. Saida no ciclo %d.\n", cicloAtual, p->id, p->tempoSaida);
                inserirFimFila(&processosFinalizados, p);
                processosConcluidos++;
            } else { // Execução normal
                printf("Ciclo %-3d: [CPU INI] Processo %2d (Prio %d) executando. (CPU: %d, I/O: %d)\n", cicloAtual, p->id, p->prio, p->tempoProc_restante, p->tempoIO_restante);
                p->tempoProc_restante -= QUANTUM_CPU;

                if (p->tempoProc_restante <= 0 && p->tempoIO_restante <= 0) {
                    p->tempoProc_restante = 1; // Prepara para o ciclo de encerramento
                    p->tempoProc_total = 0; // Flag para indicar que o processamento principal acabou
                    printf("Ciclo %-3d: [CPU FIM] Processo %2d terminou tarefas. Preparando para encerrar.\n", cicloAtual, p->id);
                    filaDeProcessamento = inserirEmFila(filaDeProcessamento, p);
                } else if (p->tempoIO_restante > 0) {
                    printf("Ciclo %-3d: [CPU FIM] Processo %2d movido para a fila de I/O.\n", cicloAtual, p->id);
                    inserirFimFila(&filaDeIO, p);
                } else {
                    printf("Ciclo %-3d: [CPU FIM] Processo %2d voltou para a fila de CPU.\n", cicloAtual, p->id);
                    filaDeProcessamento = inserirEmFila(filaDeProcessamento, p);
                }
            }
        } else {
             printf("Ciclo %-3d: [CPU OCIOSA]\n", cicloAtual);
        }

        cicloAtual++;
    }

    printf("\n--- SIMULACAO FINALIZADA NO CICLO %d ---\n", cicloAtual);
    escreverArquivoSaida(processosFinalizados);


    printf("Liberando memoria alocada...\n");
    liberarMemoria(processosFinalizados);


    return 0;
}

















