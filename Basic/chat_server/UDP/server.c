#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int iniciar_server(int porta){
    int sockfd;
    struct sockaddr_in server_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(porta);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro ao fazer bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor UDP ouvindo na porta %d\n", porta);
    return sockfd;
}

void escutar_enviar_dados(int sockfd){
    printf("Servidor UDP iniciado. Aguardando mensagens...\n");
    char buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while(1) {
        memset(buffer, 0, sizeof(buffer));

        recvfrom(sockfd, buffer, sizeof(buffer), 0,
                 (struct sockaddr *)&client_addr, &addr_len);

        printf("Cliente: %s", buffer);

        printf("Servidor: ");
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&client_addr, addr_len);
    }
    
    close(sockfd);
}


int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Uso: %s <porta>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int sockfd = iniciar_server(atoi(argv[1]));
    escutar_enviar_dados(sockfd);
    return 0;
}