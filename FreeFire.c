/*
  Nível Mestre – Organização dos Componentes da Torre de Fuga
  -----------------------------------------------------------
  O programa gerencia até 20 componentes (nome, tipo, prioridade 1..10),
  permite ordenação por:
    - nome (Bubble Sort)           -> bubbleSortNome()
    - tipo (Insertion Sort)        -> insertionSortTipo()
    - prioridade (Selection Sort)  -> selectionSortPrioridade()
  e realiza busca binária por nome (apenas após ordenar por nome).

  Métricas didáticas:
    - Número de comparações em cada algoritmo de ordenação.
    - Tempo de execução medido com clock().

  Observações:
    - Strings com espaços via fgets.
    - Comparações de strings são sensíveis a maiúsculas/minúsculas (strcmp).
    - Após cada operação relevante, a lista é exibida.
*/

#include <stdio.h>   // printf, scanf, fgets
#include <stdlib.h>  // EXIT_SUCCESS
#include <string.h>  // strcmp, strlen
#include <time.h>    // clock(), CLOCKS_PER_SEC

/* Limites e capacidades */
#define MAX_COMPONENTES 20
#define TAM_NOME 30
#define TAM_TIPO 20

/* Estrutura principal do desafio */
typedef struct {
    char nome[TAM_NOME];   /* ex.: "chip central" */
    char tipo[TAM_TIPO];   /* ex.: "controle", "suporte", "propulsao" */
    int  prioridade;       /* 1 .. 10 (1 = mais urgente/importante) */
} Componente;

/* --------------------- Utilidades de entrada/saída --------------------- */

/* Remove '\n' final, se houver. */
static void tiraQuebraDeLinha(char *s) {
    size_t n = strlen(s);
    if (n > 0 && s[n - 1] == '\n') s[n - 1] = '\0';
}

/* Lê uma linha com espaços; garante string terminada em '\0'. */
static void lerLinha(const char *prompt, char *dest, size_t max) {
    printf("%s", prompt);
    if (fgets(dest, (int)max, stdin) != NULL) {
        tiraQuebraDeLinha(dest);
    } else {
        dest[0] = '\0';
        clearerr(stdin);
    }
}

/* Lê inteiro (com limpeza do buffer) */
static int lerInteiro(const char *prompt) {
    int valor;
    int ok;
    do {
        printf("%s", prompt);
        ok = scanf("%d", &valor);
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        if (ok != 1) printf("Entrada invalida. Tente novamente.\n");
    } while (ok != 1);
    return valor;
}

/* Lê inteiro num intervalo [min..max] */
static int lerInteiroFaixa(const char *prompt, int min, int max) {
    int v;
    do {
        v = lerInteiro(prompt);
        if (v < min || v > max) {
            printf("Valor fora do intervalo (%d..%d). Tente novamente.\n", min, max);
        }
    } while (v < min || v > max);
    return v;
}

/* --------------------------- Exibição --------------------------- */

void mostrarComponentes(const Componente v[], int n) {
    printf("\n-------------------- COMPONENTES --------------------\n");
    if (n == 0) {
        printf("(vazio)\n");
    } else {
        for (int i = 0; i < n; i++) {
            printf("#%02d  Nome: %-28s  Tipo: %-12s  Prioridade: %2d\n",
                   i + 1, v[i].nome, v[i].tipo, v[i].prioridade);
        }
    }
    printf("-----------------------------------------------------\n");
}

/* ----------------------- Algoritmos de Ordenação ----------------------- */
/* Cada sort recebe um ponteiro para contador de comparações (long long*). */
/* A contagem considera comparações de chaves (strcmp / < / >).            */

/* Bubble Sort por nome (string) */
void bubbleSortNome(Componente v[], int n, long long *comparacoes) {
    *comparacoes = 0;
    int trocou;
    for (int i = 0; i < n - 1; i++) {
        trocou = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            (*comparacoes)++; /* comparação v[j].nome vs v[j+1].nome */
            if (strcmp(v[j].nome, v[j + 1].nome) > 0) {
                Componente tmp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = tmp;
                trocou = 1;
            }
        }
        if (!trocou) break; /* otimização: já está ordenado */
    }
}

/* Insertion Sort por tipo (string) */
void insertionSortTipo(Componente v[], int n, long long *comparacoes) {
    *comparacoes = 0;
    for (int i = 1; i < n; i++) {
        Componente chave = v[i];
        int j = i - 1;
        /* comparamos enquanto necessário e fazemos o shift */
        while (j >= 0) {
            (*comparacoes)++; /* comparação v[j].tipo vs chave.tipo */
            if (strcmp(v[j].tipo, chave.tipo) > 0) {
                v[j + 1] = v[j];
                j--;
            } else {
                break;
            }
        }
        v[j + 1] = chave;
    }
}

/* Selection Sort por prioridade (int) */
void selectionSortPrioridade(Componente v[], int n, long long *comparacoes) {
    *comparacoes = 0;
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            (*comparacoes)++; /* comparação de inteiros */
            if (v[j].prioridade < v[minIdx].prioridade) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            Componente tmp = v[i];
            v[i] = v[minIdx];
            v[minIdx] = tmp;
        }
    }
}

/* ------------------------- Busca Binária por Nome ------------------------- */
/* Pré-condição: vetor ordenado por nome (mesma ordem usada no Bubble).      */
/* Retorna índice [0..n-1] se encontrado, -1 caso contrário.                  */

int buscaBinariaPorNome(Componente v[], int n, const char alvo[], long long *comparacoes) {
    int l = 0, r = n - 1;
    *comparacoes = 0;
    while (l <= r) {
        int m = l + (r - l) / 2;
        (*comparacoes)++; /* comparação alvo vs v[m].nome */
        int cmp = strcmp(alvo, v[m].nome);
        if (cmp == 0) return m;
        if (cmp < 0)   r = m - 1;
        else           l = m + 1;
    }
    return -1;
}

/* ------------------------- Medição de Tempo (clock) ------------------------- */
/* Mede o tempo para um algoritmo de ordenação genérico com mesma assinatura.  */

typedef void (*SortFn)(Componente[], int, long long*);

double medirTempo(SortFn algoritmo, Componente v[], int n, long long *comparacoes) {
    clock_t t0 = clock();
    algoritmo(v, n, comparacoes);
    clock_t t1 = clock();
    return (double)(t1 - t0) / CLOCKS_PER_SEC;
}

/* ---------------------------- CRUD/Entrada ---------------------------- */

void cadastrarComponente(Componente lista[], int *qtd) {
    if (*qtd >= MAX_COMPONENTES) {
        printf("Capacidade maxima (%d) atingida.\n", MAX_COMPONENTES);
        return;
    }
    Componente c;
    lerLinha("Nome do componente: ", c.nome, TAM_NOME);
    lerLinha("Tipo (controle/suporte/propulsao/...): ", c.tipo, TAM_TIPO);
    c.prioridade = lerInteiroFaixa("Prioridade (1..10): ", 1, 10);

    lista[*qtd] = c;
    (*qtd)++;
    printf("Componente cadastrado!\n");
}

int indicePorNomeLinear(const Componente v[], int n, const char nome[]) {
    for (int i = 0; i < n; i++) {
        if (strcmp(v[i].nome, nome) == 0) return i;
    }
    return -1;
}

void removerComponente(Componente v[], int *n) {
    if (*n == 0) {
        printf("Lista vazia.\n");
        return;
    }
    char alvo[TAM_NOME];
    lerLinha("Nome do componente para remover: ", alvo, TAM_NOME);
    int idx = indicePorNomeLinear(v, *n, alvo);
    if (idx == -1) {
        printf("'%s' nao encontrado.\n", alvo);
        return;
    }
    for (int i = idx; i < *n - 1; i++) v[i] = v[i + 1];
    (*n)--;
    printf("Removido.\n");
}

/* ---------------------------- Programa Principal ---------------------------- */

int main(void) {
    Componente comp[MAX_COMPONENTES];
    int n = 0;

    int ordenadoPorNome = 0; /* flag: 1 se a última ordenação foi por nome (necessário p/ busca binária) */

    printf("=== Torre de Fuga – Modulo de Organizacao (Nivel Mestre) ===\n");
    printf("Dicas: nomes e tipos aceitam espacos; ordene por NOME antes da busca binaria.\n");

    int op;
    do {
        printf("\nMenu\n");
        printf(" 1) Cadastrar componente\n");
        printf(" 2) Remover componente (por nome)\n");
        printf(" 3) Ordenar por NOME (Bubble) e medir desempenho\n");
        printf(" 4) Ordenar por TIPO (Insertion) e medir desempenho\n");
        printf(" 5) Ordenar por PRIORIDADE (Selection) e medir desempenho\n");
        printf(" 6) Buscar BINARIA por NOME (apenas apos ordenar por nome)\n");
        printf(" 7) Listar componentes\n");
        printf(" 0) Sair\n");
        op = lerInteiro("Escolha uma opcao: ");

        if (op == 1) {
            cadastrarComponente(comp, &n);
            mostrarComponentes(comp, n);
        } else if (op == 2) {
            removerComponente(comp, &n);
            mostrarComponentes(comp, n);
        } else if (op == 3) {
            if (n <= 1) { printf("Poucos elementos para ordenar.\n"); continue; }
            long long comps = 0;
            double secs = medirTempo(bubbleSortNome, comp, n, &comps);
            ordenadoPorNome = 1;
            printf("Bubble Sort (por NOME) -> comparacoes: %lld | tempo: %.6f s\n", comps, secs);
            mostrarComponentes(comp, n);
        } else if (op == 4) {
            if (n <= 1) { printf("Poucos elementos para ordenar.\n"); continue; }
            long long comps = 0;
            double secs = medirTempo(insertionSortTipo, comp, n, &comps);
            ordenadoPorNome = 0; /* perdeu a ordenacao por nome */
            printf("Insertion Sort (por TIPO) -> comparacoes: %lld | tempo: %.6f s\n", comps, secs);
            mostrarComponentes(comp, n);
        } else if (op == 5) {
            if (n <= 1) { printf("Poucos elementos para ordenar.\n"); continue; }
            long long comps = 0;
            double secs = medirTempo(selectionSortPrioridade, comp, n, &comps);
            ordenadoPorNome = 0; /* perdeu a ordenacao por nome */
            printf("Selection Sort (por PRIORIDADE) -> comparacoes: %lld | tempo: %.6f s\n", comps, secs);
            mostrarComponentes(comp, n);
        } else if (op == 6) {
            if (!ordenadoPorNome) {
                printf("A busca binaria exige vetor ordenado por NOME. Use a opcao 3 primeiro.\n");
                continue;
            }
            if (n == 0) { printf("Inventario vazio.\n"); continue; }

            char alvo[TAM_NOME];
            lerLinha("Nome do componente-chave: ", alvo, TAM_NOME);

            long long compsBusca = 0;
            int idx = buscaBinariaPorNome(comp, n, alvo, &compsBusca);
            if (idx == -1) {
                printf("Componente-chave '%s' NAO encontrado. (comparacoes da busca: %lld)\n", alvo, compsBusca);
            } else {
                printf("Componente-chave encontrado! (comparacoes da busca: %lld)\n", compsBusca);
                printf(" -> Nome: %s | Tipo: %s | Prioridade: %d\n",
                       comp[idx].nome, comp[idx].tipo, comp[idx].prioridade);
            }
            /* montagem final visual (lista ordenada por nome) */
            mostrarComponentes(comp, n);
        } else if (op == 7) {
            mostrarComponentes(comp, n);
        } else if (op == 0) {
            printf("Encerrando. Que a torre te leve para fora da ilha!\n");
        } else {
            printf("Opcao invalida.\n");
        }

    } while (op != 0);

    return EXIT_SUCCESS;
}
