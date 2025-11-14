#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 100

int clients[MAX_CLIENTS];
pthread_mutex_t clients_lock;

void broadcast_msg(const char *msg, int exclude){
    pthread_mutex_lock(&clients_lock);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != -1 && clients[i] != exclude) {
            send(clients[i], msg, strlen(msg), 0);
        }
    }
    pthread_mutex_unlock(&clients_lock);
}

void remove_client(int sock){
    pthread_mutex_lock(&clients_lock);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == sock) {
            clients[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_lock);
    close(sock);
}

void *client_thread(void *arg){
    int sock = *(int*)arg;
    free(arg);

    char name[26];
    char buffer[1024];
    int bytes;

    send(sock, "Enter ur name: ", 17, 0);
    bytes = recv(sock, name, sizeof(name)-1, 0);
    if (bytes <=  0) {
        remove_client(sock);
        return NULL;
    }
    name[bytes] = '\0';
    
    // Remove newline do nome se existir
    char *newline = strchr(name, '\n');
    if (newline) *newline = '\0';
    newline = strchr(name, '\r');
    if (newline) *newline = '\0';

    char join_msg[128];
    char exit_msg[128];
    snprintf(join_msg, sizeof(join_msg), "%s has joined the chat.\n", name);
    snprintf(exit_msg, sizeof(exit_msg), "Press /quit to leave the chat.\n");
    
    // Envia mensagem de boas-vindas para o próprio cliente
    send(sock, join_msg, strlen(join_msg), 0);
    
    // Envia mensagem de como sair para o próprio cliente
    send(sock, exit_msg, strlen(exit_msg), 0);

    // Envia para todos os outros clientes
    broadcast_msg(join_msg, sock);

    while (1){
        bytes = recv(sock, buffer, sizeof(buffer)-1, 0);
        if (bytes <= 0) {
            break;
        }
        buffer[bytes] = '\0';

        if (strcmp(buffer, "/quit\n") == 0) {
            break;
        }

        char message[1152];
        snprintf(message, sizeof(message), "%s: %s", name, buffer);
        
        // Envia a mensagem de volta para o próprio cliente (com seu nome)
        send(sock, message, strlen(message), 0);
        
        // Envia para todos os outros clientes
        broadcast_msg(message, sock);
    }

    char leave_msg[128];
    snprintf(leave_msg, sizeof(leave_msg), "%s has left the chat.\n", name);
    broadcast_msg(leave_msg, sock);
    remove_client(sock);
    return NULL;

}

int init_server(int port){
    struct sockaddr_in server_addr;

    pthread_mutex_init(&clients_lock, NULL);
    memset(clients, 0, sizeof(clients));


    //CREATE SOCKET
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return (EXIT_FAILURE);
    }

    //CREATE SERVER ADDRESS
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    //BIND SOCKET
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        return (EXIT_FAILURE);
    }

    //LISTEN
    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        return (EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", port);
    return server_socket;
}

int connection(int server_socket){
    while (1){
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Novo client: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pthread_mutex_lock(&clients_lock);
        int stored;
        for (stored = 0; stored < MAX_CLIENTS; stored++) {
            if (clients[stored] == 0) {
                clients[stored] = client_socket;
                break;
            }
        }
        pthread_mutex_unlock(&clients_lock);

        if (stored == MAX_CLIENTS) {
            printf("Max clients reached. Connection rejected.\n");
            close(client_socket);
            continue;
        }

        pthread_t thread;
        int *pclient = malloc(sizeof(int));
        *pclient = client_socket;
        pthread_create(&thread, NULL, client_thread, pclient);
        pthread_detach(thread);

    }

    close (server_socket);
    return 0;
}



int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }

    int port = atoi(argv[1]);
    
    int server_socket = init_server(port);
    if (server_socket < 0) {
        return -1;
    }
    
    connection(server_socket);

    return 0;
}