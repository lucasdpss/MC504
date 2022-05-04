#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PEOPLE 50

sem_t m_mutex, f_mutex;
sem_t m_count, f_count;
sem_t empty;
sem_t turnstile;

void m_thread(void* v){
    printf("Thread masculina!\n");
    return;
}

void f_thread(void * v){
    printf("Thread feminina!\n");
    return;
}


int main(){
    sem_init(&m_mutex, 0, 1);
    sem_init(&f_mutex, 0, 1);
    sem_init(&m_count, 0, 3);
    sem_init(&f_count, 0, 3);
    sem_init(&empty, 0, 1);
    sem_init(&turnstile, 0, 1);

    pthread_t thr[MAX_PEOPLE];
    for(int i = 0 ; i < MAX_PEOPLE ; i++){
        int gender = random() % 2;
        void *t_func = (gender == 0) ? m_thread : f_thread;
        pthread_create(&thr[i], NULL, t_func, (void*) i);
        sleep(random() % 4);
    }

    for(int i = 0 ; i < MAX_PEOPLE ; i++){
        pthread_join(thr[i], NULL);
    }

    return 0;
}

