#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
    //Creamos los criterios de búsqueda (hints) y las direcciones de red (res)
    struct addrinfo hints, *res;
    char ipstr[INET6_ADDRSTRLEN];

    //Opciones de filtrado
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = NI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;

    //Cogemos todas las direcciones de la red o el dominio que queramos buscar
    if (getaddrinfo(argv[1], NULL, &hints, &res) != 0) {
        printf("Error: Name or service not known\n");
        return 1;
    }

    //Mostramos en consola todas las redes
    struct addrinfo* p;
    for (p = res; p != NULL; p = p->ai_next) {
        void* addr;

        //Distinguimos si es IPV4 o IPV6
        if (p->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &(ipv4->sin_addr);
        }
        else {
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        //Convertimos de binario a string la dirección de red
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("%s    %d    %d\n", ipstr, p->ai_family, p->ai_socktype);
    }

    //Liberamos memoria
    freeaddrinfo(res);

    return 0;
}