#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PEOPLE 50

sem_t m_mutex, f_mutex; // book:maleSwitch,  allows men to bar women from the room. When the first male enters, the lightswitch locks empty
sem_t m_count, f_count; // book:malMultiplex , ensure that there are no more than three men and three women in the system at a time.
sem_t empty;    //empty is 1 if the room is empty and 0 otherwise.
sem_t turnstile;

int male_count=0, female_count=0;

void f_thread(void * v){
    int id = *(int*) v;
    printf("Mulher %d entrou na fila.\n", id);

    sem_wait(&turnstile); //turnstile começa em 1, se um homem aparecer quando tiverem mulheres no banheiro, ele vai solicitar esse recurso e vai poder entrar quando empty estiver liberado. Outras mulheres que chegarem na fila vão solicitar depois, então ele vai entrar antes.
    sem_wait(&f_mutex);  //pega esse recurso impedindo que outra entre ou saia ao mesmo tempo
    female_count+=1;  //indica que mais uma mulher entrou
    if(female_count==1)
        sem_wait(&empty); //empty inicializa com 1, se essa for a primeira mulher a entrar, ela pega o recurso empty transformando em 0
    sem_post(&f_mutex);  //libera esse recurso permitindo que outra possa entrar ou sair
    sem_post(&turnstile);

    //entrada e saída do banheiro
    sem_wait(&f_count); //só entra se ainda não o recurso não tiver ido de 3 pra 0
    printf("Mulher %d entrou no banheiro.\n", id);
    sleep(random() % 6); //tempo de uso do banheiro
    sem_post(&f_count);  //sai do banheiro, liberando mais um recurso de quantidade 
    printf("Mulher %d saiu no banheiro.\n", id);
    sem_wait(&f_mutex); //pega esse recurso impedindo que outra entre ou saia ao mesmo tempo
    female_count-=1;
    if(female_count==0)
        sem_post(&empty); //se não houver mais mulheres no banheiro, libera o recurso, indicando que o banheiro está vazio
    sem_post(&f_mutex);  //libera esse recurso permitindo que outra possa entrar ou sair
    return;
}

void m_thread(void* v){
    int id = *(int*) v;
    printf("Homem %d entrou na fila.\n", id);

    sem_wait(&turnstile); 
    sem_wait(&m_mutex);  
    male_count+=1;  
    if(male_count==1)
        sem_wait(&empty); 
    sem_post(&m_mutex);  
    sem_post(&turnstile);

    //entrada e saída do banheiro
    sem_wait(&m_count); 
    printf("Homem %d entrou no banheiro.\n", id);
    sleep(random() % 6); //tempo de uso do banheiro
    sem_post(&m_count);  
    printf("Homem %d saiu no banheiro.\n", id);

    sem_wait(&m_mutex); 
    male_count-=1;
    if(male_count==0)
        sem_post(&empty); 
    sem_post(&m_mutex);  
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
        printf("i= %d\n",i);
        pthread_create(&thr[i], NULL, t_func, (void*) &i);
        sleep(random() % 4);

    }

    for(int i = 0 ; i < MAX_PEOPLE ; i++){
        pthread_join(thr[i], NULL);
    }

    return 0;
}