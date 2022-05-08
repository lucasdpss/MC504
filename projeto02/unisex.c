#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PEOPLE 50
#define PORT 8080

sem_t m_mutex, f_mutex; // book:maleSwitch,  allows men to bar women from the room. When the first male enters, the lightswitch locks empty
sem_t m_count, f_count; // book:malMultiplex , ensure that there are no more than three men and three women in the system at a time.
sem_t empty;    //empty is 1 if the room is empty and 0 otherwise.
sem_t turnstile;

// Variaveis para controlar o socket para comunicacao com a animacao
int server_fd, new_socket;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
sem_t socket_mutex; //semaforo para controlar qual thread vai enviar msg para a animacao
char msg[80];

int male_count=0, female_count=0;

void f_thread(void * v){
    int id = *(int*) v;

    sem_wait(&socket_mutex);
    printf("Mulher %d entrou na fila.\n", id);
    sprintf(msg, "Mulher %d entrou na fila.\n", id);
    send(new_socket, msg, strlen(msg), 0);
    sem_post(&socket_mutex);

    sem_wait(&turnstile); //turnstile começa em 1, se um homem aparecer quando tiverem mulheres no banheiro, ele vai solicitar esse recurso e vai poder entrar quando empty estiver liberado. Outras mulheres que chegarem na fila vão solicitar depois, então ele vai entrar antes.
    sem_wait(&f_mutex);  //pega esse recurso impedindo que outra entre ou saia ao mesmo tempo
    female_count+=1;  //indica que mais uma mulher entrou
    if(female_count==1)
        sem_wait(&empty); //empty inicializa com 1, se essa for a primeira mulher a entrar, ela pega o recurso empty transformando em 0
    sem_post(&f_mutex);  //libera esse recurso permitindo que outra possa entrar ou sair
    sem_post(&turnstile);

    //entrada e saída do banheiro
    sem_wait(&f_count); //só entra se ainda não o recurso não tiver ido de 3 pra 0

    sem_wait(&socket_mutex);
    printf("Mulher %d entrou no banheiro.\n", id);
    sprintf(msg, "Mulher %d entrou no banheiro.\n", id);
    send(new_socket, msg, strlen(msg), 0);
    sem_post(&socket_mutex);


    sleep((random() % 4) + 3); //tempo de uso do banheiro
    sem_post(&f_count);  //sai do banheiro, liberando mais um recurso de quantidade 

    sem_wait(&socket_mutex);
    printf("Mulher %d saiu do banheiro.\n", id);
    sprintf(msg, "Mulher %d saiu do banheiro.\n", id);
    send(new_socket, msg, strlen(msg), 0);
    sem_post(&socket_mutex);


    sem_wait(&f_mutex); //pega esse recurso impedindo que outra entre ou saia ao mesmo tempo
    female_count-=1;
    if(female_count==0)
        sem_post(&empty); //se não houver mais mulheres no banheiro, libera o recurso, indicando que o banheiro está vazio
    sem_post(&f_mutex);  //libera esse recurso permitindo que outra possa entrar ou sair
    return;
}

void m_thread(void* v){
    int id = *(int*) v;

    sem_wait(&socket_mutex);
    printf("Homem %d entrou na fila.\n", id);
    sprintf(msg, "Homem %d entrou na fila.\n", id);
    send(new_socket, msg, strlen(msg), 0);
    sem_post(&socket_mutex);

    sem_wait(&turnstile); 
    sem_wait(&m_mutex);  
    male_count+=1;  
    if(male_count==1)
        sem_wait(&empty); 
    sem_post(&m_mutex);  
    sem_post(&turnstile);

    //entrada e saída do banheiro
    sem_wait(&m_count); 

    sem_wait(&socket_mutex);
    printf("Homem %d entrou no banheiro.\n", id);
    sprintf(msg, "Homem %d entrou no banheiro.\n", id);
    send(new_socket, msg, strlen(msg), 0);
    sem_post(&socket_mutex);


    sleep((random() % 4) + 3); //tempo de uso do banheiro
    sem_post(&m_count);

    sem_wait(&socket_mutex);
    printf("Homem %d saiu do banheiro.\n", id);
    sprintf(msg, "Homem %d saiu do banheiro.\n", id);
    send(new_socket, msg, strlen(msg), 0);
    sem_post(&socket_mutex);  

    sem_wait(&m_mutex); 
    male_count-=1;
    if(male_count==0)
        sem_post(&empty); 
    sem_post(&m_mutex);  
    return;
}


int main(){

    // Criando o socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
        == 0) {
        printf("ERROR: socket failed");
    }
 
    // Definindo o socket para a porta 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        printf("ERROR: setsockopt");
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    // Fazer o bind do socket para a porta 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        printf("ERROR: bind failed");
    }
    if (listen(server_fd, 3) < 0) {
        printf("ERROR: listening");
    }

    // Espera uma conexao do cliente com a janela grafica
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
            printf("ERROR: accept");
    }

    sem_init(&m_mutex, 0, 1);
    sem_init(&f_mutex, 0, 1);
    sem_init(&m_count, 0, 3);
    sem_init(&f_count, 0, 3);
    sem_init(&empty, 0, 1);
    sem_init(&turnstile, 0, 1);
    sem_init(&socket_mutex, 0, 1);

    pthread_t thr[MAX_PEOPLE];
    for(int i = 0 ; i < MAX_PEOPLE ; i++){
        int gender = random() % 2;
        void *t_func = (gender == 0) ? m_thread : f_thread;
        //printf("i= %d\n",i);
        pthread_create(&thr[i], NULL, t_func, (void*) &i);
        sleep(1 + random()%3);

    }

    for(int i = 0 ; i < MAX_PEOPLE ; i++){
        pthread_join(thr[i], NULL);
    }

    return 0;
}