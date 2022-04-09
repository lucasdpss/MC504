#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define QUAD_WDT 5
#define QUAD_HGT 5

typedef struct {
    char **matriz;
    int M;
    int N;
    int i;
    int j;
} f_thread_args;

void* f_thread(void *void_args) {
    f_thread_args *args = (f_thread_args*)void_args;
    // printf("%d %d\n", args.i, args.j);
    free(&args);
    return NULL; 
}

char **AlocaM(int m, int n){
  char **M;
  int i;
 
  M = (int **)malloc(sizeof(int *)*m);
  if(M == NULL){
    printf("Memoria insuficiente.\n");
    exit(1);
  }
  for(i = 0; i < m; i++){
    M[i] = (int *)malloc(sizeof(int)*n);
    if(M[i] == NULL){
      printf("Memoria insuficiente.\n");
      exit(1);
    }
  }
  return M;
} 

void print_matriz(char **matriz, int M, int N){
    printf("entrou\n");
    
    for(int i = 0 ; i < M ; i++){
        for(int j = 0 ; j < N ; j++){
            printf("%c", matriz[i][j]);
        }
        printf("\n");
    }
}


int main() {
    printf("lalallalalal\n");
    int M, N;
    scanf("%d %d\n", &M, &N);
    char **matriz = AlocaM(M,N);3
    

    // char **matriz = malloc(M * sizeof(char*));
    // for(int i = 0 ; i < M ; i++){
    //     matriz[i] = malloc(N * sizeof(char));
    // }
    // printf("para\n");

    for(int i = 0 ; i < M ; i++){
        for(int j = 0 ; j < N ; j++){
            scanf("%c ", &matriz[i][j]);
            printf("%s/%d/%d\n ",&matriz[i][j],i,j);

        }
    }
    printf("entrou\n");

    print_matriz(matriz, M, N);

    const int thr_num_hgt = M / QUAD_HGT + ((M % QUAD_HGT == 0) ? 0 : 1); // teto(M / QUAD_HGT)
    const int thr_num_wdt = N / QUAD_WDT + ((N % QUAD_WDT == 0) ? 0 : 1); // teto(N / QUAD_WDT)
    pthread_t thr[thr_num_hgt][thr_num_wdt];

    for(int i = 0 ; i < thr_num_hgt ; i++){
        for(int j = 0 ; j < thr_num_wdt ; j++){
            f_thread_args* args = malloc(sizeof(f_thread_args));
            args->matriz = matriz;
            args->M = M;
            args->N = N;
            args->i = i;
            args->j = j;
            pthread_create(&thr[i][j], NULL, f_thread, (void*) args);
        }
    }

    
/*
    pthread_t thr[N_THR];
    int i;

    for (i = 0; i < N_THR; i++) 
    pthread_create(&thr[i], NULL, f_thread, (void*) i);

    for (i = 0; i < N_THR; i++) 
        pthread_join(thr[i], NULL); 
*/
    for(int i = 0 ; i < thr_num_hgt ; i++){
        for(int j = 0 ; j < thr_num_wdt ; j++){
            pthread_join(thr[i][j], NULL);
        }
    }

    for(int i = 0 ; i < M ; i++){
        free(matriz[i]);
    }
    free(matriz);
    return 0;
}
