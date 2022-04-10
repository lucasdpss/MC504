#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define QUAD_WDT 5
#define QUAD_HGT 5

typedef struct {
    char **matriz;
    char **prox_matriz;
    int M;
    int N;
    int i;
    int j;
} f_thread_args;

int get_vizinhos_vivos(char **matriz, int M, int N, int i, int j){
    int count = 0;

    for(int x = i - 1 ; x <= i+1 ; x++){
        for(int y = j - 1 ; y <= j+1 ; y++){
            if(x < 0 || x >= M || y < 0 || y >= N) continue; // Celula fora da matriz
            if(x == i && y == j) continue; // Propria celula
            printf("Lendo [%d][%d] como %s\n", x, y, (matriz[x][y] == '#' ? "vivo" : "morto"));
            count += (matriz[x][y] == '#' ? 1 : 0);
        }
    }

    return count;
}

void* f_thread(void *void_args) {
    f_thread_args *args = (f_thread_args*) void_args;
    for(int x = args->i * QUAD_HGT ; x < (args->i + 1) * QUAD_HGT ; x++){
        for(int y = args->j * QUAD_WDT ; y < (args->j + 1) * QUAD_WDT ; y++){
            if(x < 0 || x >= args->M || y < 0 || y >= args->N) continue; // Celula fora da matriz
            // Processa celula
        }
    }

    free(args);
    return NULL; 
}

char** malloc_matriz(int M, int N){
  char **matriz;
  int i;
 
  matriz = (char**) malloc(sizeof(char*) * M);
  if(matriz == NULL){
    printf("Memoria insuficiente.\n");
    exit(1);
  }
  for(i = 0; i < M; i++){
    matriz[i] = (char*) malloc(sizeof(char) * N);
    if(matriz[i] == NULL){
      printf("Memoria insuficiente.\n");
      exit(1);
    }
  }
  return matriz;
} 

void free_matriz(char **matriz, int M){
    for(int i = 0 ; i < M ; i++){
        free(matriz[i]);
    }
    free(matriz);
}

void print_matriz(char **matriz, int M, int N){    
    for(int i = 0 ; i < M ; i++){
        for(int j = 0 ; j < N ; j++){
            printf("%c", matriz[i][j]);
        }
        printf("\n");
    }
}


int main() {
    int M, N;
    scanf("%d %d\n", &M, &N);
    char **matriz = malloc_matriz(M,N);

    for(int i = 0 ; i < M ; i++){
        for(int j = 0 ; j < N ; j++){
            scanf("%c ", &matriz[i][j]);
        }
    }

    printf("%d\n", get_vizinhos_vivos(matriz, M, N, 2, 1));

    print_matriz(matriz, M, N);

    const int thr_num_hgt = M / QUAD_HGT + ((M % QUAD_HGT == 0) ? 0 : 1); // teto(M / QUAD_HGT)
    const int thr_num_wdt = N / QUAD_WDT + ((N % QUAD_WDT == 0) ? 0 : 1); // teto(N / QUAD_WDT)
    pthread_t thr[thr_num_hgt][thr_num_wdt];

    for(int i = 0 ; i < thr_num_hgt ; i++){
        for(int j = 0 ; j < thr_num_wdt ; j++){
            f_thread_args* args = malloc(sizeof(f_thread_args));
            args->matriz = matriz;
            args->prox_matriz = malloc_matriz(M, N);
            args->M = M;
            args->N = N;
            args->i = i;
            args->j = j;
            pthread_create(&thr[i][j], NULL, f_thread, (void*) args);
        }
    }

    
    for(int i = 0 ; i < thr_num_hgt ; i++){
        for(int j = 0 ; j < thr_num_wdt ; j++){
            pthread_join(thr[i][j], NULL);
        }
    }

    free_matriz(matriz, M);
    return 0;
}
