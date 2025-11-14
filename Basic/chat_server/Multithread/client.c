#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

void *recv_thread(void *arg) {
    int sock = *(int*)arg;
    free(arg);
    char buffer[1024];
    int bytes;

    while((bytes = recv(sock, buffer, sizeof(buffer)-1, 0)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }
    return NULL;
}

int create_socket(int port){

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0){
        perror("Socket creation failed");
        return -1;
    }
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        close(sock);
        return -1;
    }
    return sock;
}

int receive_msg(int sock){
    pthread_t thread;
    int *sock_ptr = malloc(sizeof(int));
    *sock_ptr = sock;
    pthread_create(&thread, NULL, recv_thread, sock_ptr);
    pthread_detach(thread);
    
    // Pequeno delay para garantir que a thread está pronta
    usleep(100000); // 100ms
    
    char msg[1024];
    while(1) {
        fgets(msg, sizeof(msg), stdin);
        send(sock, msg, strlen(msg), 0);
        if(strcmp(msg, "/quit\n") == 0) break;
    }

    close(sock);
    return 0;
}

int main (int argc, char *argv[]){
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }

    int port = atoi (argv[1]);
    int sock = create_socket(port);
    receive_msg(sock);

    return 0;
}