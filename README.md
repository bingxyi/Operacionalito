# Operacionalito 🔧
Simulador de Escalonador de Processos com Prioridade

Este projeto é uma simulação em linguagem C de um escalonador de processos para um sistema operacional simplificado. O programa gerencia a execução de múltiplos processos com base em um algoritmo de prioridade preenptiva, lidando com operações de CPU e I/O (Entrada/Saída).

# Funcionalidades

    Leitura de Processos: Carrega uma lista de processos a partir de um arquivo processos.txt.

    Escalonamento por Prioridade: Utiliza uma fila de prioridades para determinar qual processo deve usar a CPU. Processos com menor número de prioridade são executados primeiro.

    Simulação de CPU e I/O: Gerencia o tempo de processamento e de I/O de cada processo, movendo-os entre as filas de CPU e I/O.

    Fatias de Tempo (Quantum): Concede fatias de tempo fixas para uso da CPU (3 ciclos) e I/O (6 ciclos).

    Geração de Relatório: Ao final da simulação, gera um arquivo saida.txt com o ID de cada processo e seu tempo de finalização no sistema.

    Gerenciamento de Memória: Realiza a alocação dinâmica de memória para os processos e a libera ao final da execução para evitar vazamentos de memória.

# Como Funciona ⚙️

O sistema opera em um laço principal que simula ciclos de clock. A cada ciclo, o simulador realiza as seguintes ações:

    Verificação de Entrada: Checa se algum processo deve entrar no sistema com base no seu tempoEntrada. Se sim, o processo é inserido na Fila de Processamento.

    Fila de Processamento (CPU):

        É uma fila de prioridade. O processo com o menor valor no campo prio é sempre o primeiro.

        O processo no topo da fila recebe uma fatia de tempo de CPU (QUANTUM_CPU).

    Fila de I/O:

        É uma fila comum (FIFO - Primeiro a Entrar, Primeiro a Sair), sem prioridades.

        Processos que necessitam de operações de I/O são movidos para esta fila.

        Ao receber sua fatia de I/O (QUANTUM_IO), o processo retorna à fila de processamento para continuar sua execução.

    Ciclo de Vida de um Processo:

        Um processo alterna entre as filas de CPU e I/O até que seu tempoProc_restante e tempoIO_restante sejam zerados.

        Ao finalizar todas as suas tarefas, o processo utiliza um último ciclo de clock para seu encerramento e é movido para uma lista de finalizados.

# Estrutura do Código 📂

O código é estruturado em torno da struct Processo e de um conjunto de funções que gerenciam a simulação e as filas.
Função	Descrição
main()	Orquestra toda a simulação, controla o avanço do clock e chama as funções de gerenciamento.
readFile()	Lê o arquivo processos.txt e cria a lista inicial de processos na memória.
inserirEmFila()	Insere um processo na fila de CPU, mantendo a ordem de prioridade.
inserirFimFila()	Insere um processo no final da fila de I/O (lógica FIFO).
removerDaFila()	Remove o primeiro processo de uma fila para que ele possa ser executado.
escreverArquivoSaida()	Gera o arquivo saida.txt com os resultados da simulação.
liberarMemoria()	Percorre a lista de processos finalizados e libera toda a memória alocada.

# Como Compilar e Executar ▶️

Pré-requisitos

    Um compilador C (como o GCC).

    O arquivo processos.txt no mesmo diretório do código-fonte.

Arquivo processos.txt

Crie um arquivo chamado processos.txt onde cada linha representa um processo, com os seguintes atributos separados por ponto e vírgula:
id;tempoEntrada;tempoIO;tempoProcessamento;prioridade

Exemplo:

1;1;5;12;3
2;2;4;8;2
3;4;3;15;1

Passos para Execução

    Compile o código através do terminal:

gcc seu_arquivo.c -o simulador

Execute o programa compilado:

```Bash
./simulador
```

Verifique os resultados:

    O console exibirá um log detalhado de cada ciclo da simulação.

    Um arquivo saida.txt será criado com o tempo de saída de cada processo.
