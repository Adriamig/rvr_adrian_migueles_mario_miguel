#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[])
{
    //Creamos los criterios de búsqueda (hints) y las direcciones de red (res)
    struct addrinfo hints, *res;
    char ipstr[INET6_ADDRSTRLEN];

    //Opciones de filtrado
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    //Cogemos todas las direcciones de la red o el dominio que queramos buscar
    if (getaddrinfo(argv[1], argv[2], &hints, &res) != 0) {
        printf("Error: Name or service not known\n");
        return 1;
    }

    //Abrimos un socket
    int sd = socket(res->ai_family, res->ai_socktype, 0);
    //Asociamos la dirección del servidor
    bind(sd, (struct sockaddr*)res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    //Comunicación entre servidor-cliente
    char* buffer;
    struct sockaddr_storage* cliente;
    socklen_t cliente_len = sizeof(cliente);
    while (1) {
        //Cogemos el socket del cliente
        int client_sd = accept(sd, (struct sockaddr*)&cliente, &cliente_len);
        //Recibimos mensaje del cliente
        ssize_t bytes = recvfrom(sd, buffer, 80, 0, (struct sockaddr*)&cliente,
            &cliente_len);
        buffer[bytes] = '\0';

        //Enseñamos por pantalla quién es el cliente y cuántos bytes nos envía
        char host[NI_MAXHOST], serv[NI_MAXSERV];
        if (getnameinfo((struct sockaddr*)&cliente, cliente_len, host, NI_MAXHOST,
            serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) != 0) {
            printf("Error: Name or service not known\n");
        }
        else {
            printf("%d bytes de %s:%s\n", bytes, host, serv);
        }

        //Dependiendo del mensaje del cliente, enviamos su correspondiente
        switch (buffer[0]) {
        //Horas
        case 't': {
            time_t t = time(NULL);
            struct tm* tm = localtime(&t);
            char response[1024];
            strftime(response, sizeof(response), "%H:%M:%S", tm);
            sendto(sd, response, strlen(response), 0, (struct sockaddr*)&cliente, cliente_len);
            break;
        }
        //Fecha
        case 'd': {
            time_t t = time(NULL);
            struct tm* tm = localtime(&t);
            char response[1024];
            strftime(response, sizeof(response), "%Y-%m-%d", tm);
            sendto(sd, response, strlen(response), 0, (struct sockaddr*)&cliente, cliente_len);
            break;
        }
        //Cierra cliente-servidor
        case 'q': {
            printf("Saliendo...\n");
            close(client_sd);
            close(sd);
            exit(EXIT_SUCCESS);
            break;
        }
        //Mensaje sin correspondencia
        default: {
            printf("Comando no soportado %s\n", buffer);
            break;
        }
        }
    }
}