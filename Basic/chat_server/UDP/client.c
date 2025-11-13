#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int iniciar_cliente(int porta, struct sockaddr_in *server){
    int sockfd;
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    memset(server, 0, sizeof(*server));
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = inet_addr("127.0.0.1");
    server->sin_port = htons(porta);
    
    return sockfd;
}

int enviar_receber_mensagens(int sockfd, struct sockaddr_in *server){
    char mensagem[1024];
    socklen_t addr_len = sizeof(*server);

        while (1) {
        printf("Cliente: ");
        fgets(mensagem, sizeof(mensagem), stdin);

        sendto(sockfd, mensagem, strlen(mensagem), 0,
               (struct sockaddr *)server, addr_len);

        memset(mensagem, 0, sizeof(mensagem));

        recvfrom(sockfd, mensagem, sizeof(mensagem), 0,
                 (struct sockaddr *)server, &addr_len);

        printf("Servidor: %s", mensagem);
    }

    close(sockfd);
    return sockfd;
}


int main (int argc, char *argv[]){
    if (argc != 2) {
        printf("Uso: %s <porta>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server;
    int sockfd = iniciar_cliente(atoi(argv[1]), &server);
    enviar_receber_mensagens(sockfd, &server);

    return 0;
}