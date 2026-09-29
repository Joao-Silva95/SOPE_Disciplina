#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define TAMANHO 100000000 // 100 milhões de elementos
#define NUM_PROCESSOS 4

int *vetor;

// Função para medir o tempo em segundos
double tempo_atual() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

// 1. CÁLCULO SEQUENCIAL
long long soma_sequencial() {
    long long soma = 0;
    for (int i = 0; i < TAMANHO; i++) {
        soma += vetor[i];
    }
    return soma;
}

// 2. CÁLCULO PARALELO (usando múltiplos processos e ficheiros via POSIX)
long long soma_paralela() {
    int fatias = TAMANHO / NUM_PROCESSOS;
    pid_t pids[NUM_PROCESSOS];

    for (int i = 0; i < NUM_PROCESSOS; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("Erro no fork");
            exit(1);
        }

        // --- CÓDIGO DO PROCESSO FILHO ---
        if (pids[i] == 0) {
            int inicio = i * fatias;
            int fim;

            if (i == NUM_PROCESSOS - 1) {
                fim = TAMANHO;
            } else {
                fim = inicio + fatias;
            }

            long long soma_parcial = 0;
            for (int j = inicio; j < fim; j++) {
                soma_parcial += vetor[j];
            }

            // Gerar o nome do ficheiro para cada filho
            char nome_ficheiro[32];
            snprintf(nome_ficheiro, sizeof(nome_ficheiro), "parcial_%d.tmp", i);

            // Chamada POSIX: open()
            int fd = open(nome_ficheiro, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd >= 0) {
                // Chamada POSIX: write()
                write(fd, &soma_parcial, sizeof(soma_parcial));
                // Chamada POSIX: close()
                close(fd);
            }

            // O filho termina com exit()
            exit(0);
        }
    }

    // --- CÓDIGO DO PROCESSO PAI ---
    long long soma_total = 0;

    for (int i = 0; i < NUM_PROCESSOS; i++) {
        int status;
        // Chamada POSIX: waitpid()
        waitpid(pids[i], &status, 0);

        char nome_ficheiro[32];
        snprintf(nome_ficheiro, sizeof(nome_ficheiro), "parcial_%d.tmp", i);

        // Chamada POSIX: open() para leitura
        int fd = open(nome_ficheiro, O_RDONLY);
        if (fd >= 0) {
            long long parcial = 0;
            // Chamada POSIX: read()
            read(fd, &parcial, sizeof(parcial));
            soma_total += parcial;
            close(fd);

            // Chamada POSIX: unlink() (remove o ficheiro temporário)
            unlink(nome_ficheiro);
        }
    }

    return soma_total;
}

int main() {
    printf("A alocar memória e a preencher vetor com %d números aleatórios [0-100]...\n", TAMANHO);
    vetor = malloc(sizeof(int) * TAMANHO);

    // Inicialização da semente para números aleatórios
    srand(time(NULL));

    // Preenchimento com números aleatórios entre 0 e 100
    for (int i = 0; i < TAMANHO; i++) {
        vetor[i] = rand() % 101;
    }

    // --- Teste Sequencial ---
    double inicio_seq = tempo_atual();
    long long res_seq = soma_sequencial();
    double fim_seq = tempo_atual();
    double tempo_seq = fim_seq - inicio_seq;

    printf("\n[Sequencial] Resultado: %lld | Tempo: %.4f segundos\n", res_seq, tempo_seq);

    // --- Teste Paralelo ---
    double inicio_par = tempo_atual();
    long long res_par = soma_paralela();
    double fim_par = tempo_atual();
    double tempo_par = fim_par - inicio_par;

    printf("[Paralelo  ] Resultado: %lld | Tempo: %.4f segundos\n", res_par, tempo_par);

    // --- Evidência do Desempenho ---
    printf("\n-> Aumento de Desempenho (Speedup): %.2fx mais rápido!\n", tempo_seq / tempo_par);

    free(vetor);
    return 0;
}
