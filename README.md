# Projeto "Operacionalito"

### 🔧 Simulação de escalonamento de processos com sistema de prioridade
Este projeto é uma simulação de escalonamento de processos em linguagem C, utilizando filas de prioridade e filas FIFO para CPU e I/O.

O objetivo é reproduzir, de forma simplificada, o funcionamento de um sistema operacional que gerencia múltiplos processos concorrentes, respeitando prioridades, fatias de tempo (quantum) e ciclos de I/O.


## 📂 Estrutura do Projeto
```bash
.
├── main.c          # Código-fonte da simulação
├── processos.txt   # Arquivo de entrada com os processos
└── saida.txt       # Arquivo de saída gerado após a simulação
```


## 📑 Estrutura do Código
### 🔹 struct Processo

Cada processo é representado pela seguinte estrutura:
```C
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
```
### 🔹 Funções Principais

- Gerenciamento de filas

    - `inserirEmFila` → Insere um processo em uma fila ordenada por prioridade.

    - `removerDaFila` → Remove o processo da cabeça da fila.

    - `inserirFimFila` → Insere um processo no fim da fila (usado para I/O e lista de espera).

- Entrada e saída

    - `readFile` → Lê os processos do arquivo processos.txt.

    - `escreverArquivoSaida` → Gera o arquivo saida.txt com os processos finalizados.

- Gerenciamento de memória

    - liberarMemoria → Libera memória alocada dinamicamente ao final da execução.

- Loop principal da simulação

    - Entrada de novos processos no sistema.

    - Execução de I/O com fatia de tempo definida.

    - Escalonamento de CPU considerando prioridade e quantum.

    - Encerramento de processos e registro no log.


## 📜 Arquivo de Entrada (processos.txt)

O arquivo de entrada deve conter uma lista de processos no seguinte formato:
```bash
id;tempoEntrada;tempoIO_total;tempoProc_total;prioridade
```
Exemplo usado no projeto:
```txt
1;1;5;12;3
2;2;4;8;2
3;4;3;15;1
4;6;0;7;4
5;8;6;10;5
6;10;2;14;3
7;12;8;7;2
8;14;5;11;4
9;16;3;13;1
10;18;6;9;5
11;20;4;12;2
12;22;7;8;3
13;24;2;15;4
14;26;5;10;1
15;28;3;14;5
```


## ⚙️ Funcionamento da Simulação

- Quantum da CPU: 3 ciclos

- Quantum de I/O: 6 ciclos

Prioridades: quanto menor o número, maior a prioridade (1 é mais urgente, 5 é menos).

### 🔄 Fluxo da simulação

1. Processos entram no sistema no ciclo indicado.

2. Processos vão para a fila da CPU (prioridade).

3. Quando precisam de I/O, são enviados para a fila de I/O (FIFO).

4. Se concluírem o I/O, retornam à CPU ou finalizam se não houver mais trabalho.

5. Processos são encerrados e registrados em saida.txt.


## ▶️ Como Compilar e Executar

### 🔹 Compilar
No terminal:
```bash
gcc main.c -o filaMain
```

### 🔹 Executar
```bash
./filaMain
```
- O programa irá:

    - Ler processos.txt.

    - Simular o escalonamento.

    - Exibir logs no terminal.

    - Gerar saida.txt com o resultado final.
_________
