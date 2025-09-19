#include <stdio.h>
#include <stdlib.h>

#define MAX 15

struct Processo {
    int id; // ID do processo
    int burstTime;      // Tempo necessário para um processo concluir a sua execução na CPU
    int remainingTime;      // Tempo restante
};

// Função para realizar o agendamento de processo do Round Robin
void roundRobin(struct Processo processos[], int n, int timeQuantum) {
    int time = 0;   // Rastreia o tempo atual da fila
    int completed = 0;
    int waitingTime = 0, turnAroundTime = 0;
    int processosQueue[MAX];
    int front = 0, rear = 0;

    printf("---Iniciando a execucao do Round Robin---\n");
    printf("Time Quantum: %d segundos\n\n", timeQuantum);

    // Fazendo um array para assistir o processo
    while (completed < n) {
        int processRanInThisCycle = 0;   // Flag para verificar se algum processo rodou no ciclo

        // Loop for para percorrer todos os processos para ver qual deve ser executado
        for (int i = 0; i < n; i++) {
            // Caso o processo ainda possua tempo restante, o mesmo permanecerá na fila para ser executado
            if (processos[i].remainingTime > 0) {
                processRanInThisCycle = 1;  // Marca que o processo foi encontrado
                printf("Tempo atual: %d | Processo %d selecionado (Restam: %d segundos)\n", time, processos[i].id, processos[i].remainingTime);

                // Caso 1: O processo precisa de mais tempo do que o time quantum (tempo quantico)
                if (processos[i].remainingTime > timeQuantum) {
                    processos[i].remainingTime -= timeQuantum;
                    time += timeQuantum;

                    printf(" -> Processo %d executou por %d segundos | Tempo restante: %d\n\n", processos[i].id, timeQuantum, processos[i].remainingTime);
                }
                // Caso 2: O processo precisa de mais ou menos o mesmo tempo que o time Quantum
                else {
                    // Avança o tempo total pelo tempo que faltava para o processo
                    time += processos[i].remainingTime;

                    // O tempo de espera para este processo é o tempo final menos seu burst time original
                    waitingTime += time - processos[i].burstTime;

                    // O tempo de turnaround para este processo é o tempo total que ele levou no sistema
                    turnAroundTime += time;

                    printf(" -> Processo %d executou por %d segundos e foi concluido!\n", processos[i].id, processos[i].remainingTime);
                    printf(" -> Tempo total para o sistema P%d: %d segundos\n\n", processos[i].id, time);

                    // Zera o tempo restante e marca como processo completo
                    processos[i].remainingTime = 0;
                    completed++;
                }
            }
        }
        // Caso nenhum processo rodou neste ciclo, significa que a CPU esta ociosa
        // (E mais relevante quando processos possuem tempos de chegada diferentes)
        if (!processRanInThisCycle) {
            time++;
        }
    }

    printf("---Execucao finalizada!---");
    // Conversão de valores de int para float => (float)
    printf("\nTempo de espera aproximadamente = %.2f segundos\n", (float)waitingTime / n);
    printf("Tempo de 'TurnAround' aproximadamente = %.2f segundos\n", (float) turnAroundTime / n);
}

// Função main
int main() {
    struct Processo processos[MAX];
    int n, timeQuantum;         // n = número de processos && timeQuantum = fatia de tempo que cada processo receberá do clock

    printf("Insira o numero de processos de 1 a 15: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        processos[i].id = i + 1;
        printf("Insira o tempo necessario para a conclusao do processo %d: ", i + 1);
        scanf("%d", &processos[i].burstTime);

        processos[i].remainingTime = processos[i].burstTime; // Inicializa o tempo restante
    }

    printf("Insira o tempo quantico, ou seja, a fatia de tempo do processador: ");
    scanf("%d", &timeQuantum);

    // rodando o round robin
    roundRobin(processos, n, timeQuantum);
    return 0;
}




















