#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080


int main(int argc, char const* argv[]){
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char* hello = "Hello from server 00 01 02 03\n";
 
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
        == 0) {
        printf("ERROR: socket failed");
    }
 
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        printf("ERROR: setsockopt");
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        printf("ERROR: bind failed");
    }
    if (listen(server_fd, 3) < 0) {
        printf("ERROR: listening");
    }
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
            printf("ERROR: accept");
    }

    //valread = read(new_socket, buffer, 1024);
    //printf("%s\n", buffer);

    for(int i=0; i < 10; i++){
        send(new_socket, hello, strlen(hello), 0);
        printf("%s\n", hello);
        sleep(3);
    }

    printf("Finished\n");
    return 0;
}