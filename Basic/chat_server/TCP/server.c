#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <string.h> //strlen, strcmp
#include <unistd.h> //close, read, write
#include <sys/socket.h> //socket, bind, listen, accept
#include <arpa/inet.h> //inet_addr, inet_ntoa

void cabecalho(){
    printf("=====================================\n");
    printf("=       TCP Chat Server v1.0        =\n");
    printf("=         Author: etih4rd           =\n");
    printf("=====================================\n");
}

int iniciar_servidor(int porta, struct sockaddr_in *server_addr){

    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(porta);
    
    if (bind(sockfd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        perror("ERROR ao fazer bind");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

int escutar_conexoes(int sockfd, int backlog, struct sockaddr_in server_addr){

    if (listen(sockfd, backlog) < 0) {
        perror("ERROR ao escutar conexoes");
        exit(EXIT_FAILURE);
    }

    printf("Servidor ouvindo na porta %d\n", ntohs(server_addr.sin_port));
    
    int client_fd;
    if ((client_fd = accept(sockfd, NULL, NULL)) < 0) {
        perror("ERROR ao aceitar conexao");
        exit(EXIT_FAILURE);
    }
    return client_fd;
}

void enviar_receber_dados(int sockfd, int client_fd){

    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        recv(client_fd, buffer, sizeof(buffer), 0);
        printf("Cliente: %s", buffer);

        printf("Servidor: ");
        fgets(buffer, sizeof(buffer), stdin);
        send(client_fd, buffer, strlen(buffer), 0);
    }
    close(client_fd);
    close(sockfd);
}

int main (int argc, char *argv[]){

    
    cabecalho();
    struct sockaddr_in server_addr;
    int sockfd = iniciar_servidor(atoi(argv[1]), &server_addr);
    int client_fd = escutar_conexoes(sockfd, 5, server_addr);
    enviar_receber_dados(sockfd, client_fd);

    return 0;
}