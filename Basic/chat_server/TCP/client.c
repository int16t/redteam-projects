#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int iniciar_cliente(int porta, struct sockaddr_in *server){
    int sockfd;
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }
    
    memset(server, 0, sizeof(*server));
    server->sin_family = AF_INET;
    server->sin_port = htons(porta);
    server->sin_addr.s_addr = inet_addr("127.0.0.1");
    
    return sockfd;
}

int conectar_servidor(int sockfd, struct sockaddr_in server){
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Erro ao conectar ao servidor");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void enviar_receber_dados(int sockfd){
    char buffer[1024];
    while (1) {
        printf("Cliente: ");
        fgets(buffer, sizeof(buffer), stdin);
        send(sockfd, buffer, strlen(buffer), 0);

        memset(buffer, 0, sizeof(buffer));
        recv(sockfd, buffer, sizeof(buffer), 0);
        printf("Servidor: %s", buffer);
    }

    close(sockfd);

}


int main(int argc, char *argv[]){

    struct sockaddr_in server;
    int sockfd = iniciar_cliente(atoi(argv[1]), &server);
    sockfd = conectar_servidor(sockfd, server);
    enviar_receber_dados(sockfd);

    return 0;
}