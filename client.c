#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT 3500    /* Palvelimen TCP-portti */
#define MYUDPPORT 4950     /* Asiakkaan UDP-kuunteluportti */

int main(int argc, char *argv[])
{
    int sockfd, udp_fd, numbytes, addr_len;
    char buf[101];
    struct hostent *he;
    struct sockaddr_in server_addr;
    struct sockaddr_in my_udp_addr;
    struct sockaddr_in from_addr;

    if (argc != 3) {
        printf("Käyttö: %s <isäntänimi> <teksti>\n", argv[0]);
        exit(1);
    }

    /* Selvitetään palvelimen IP-osoite isäntänimen perusteella */
    he = gethostbyname(argv[1]);

    /* Luodaan UDP-soketti vastauksen vastaanottamista varten */
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);

    my_udp_addr.sin_family = AF_INET;
    my_udp_addr.sin_port = htons(MYUDPPORT);
    my_udp_addr.sin_addr.s_addr = INADDR_ANY;

    /* Sidotaan UDP-soketti tiettyyn kuunteluporttiin (4950) */
    bind(udp_fd, (struct sockaddr *)&my_udp_addr, sizeof(struct sockaddr));

    /* Luodaan TCP-soketti viestin lähetystä varten */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVERPORT);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);

    /* Muodostetaan TCP-yhteys palvelimeen */
    connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

    /* Lähetetään käyttäjän antama teksti (argv[2]) TCP-yhteydellä */
    send(sockfd, argv[2], strlen(argv[2]), 0);

    /* Suljetaan TCP-yhteys lähetysten jälkeen */
    close(sockfd);

    /* Odotetaan isokirjaimista vastausta palvelimelta UDP-soketin kautta */
    addr_len = sizeof(struct sockaddr);
    numbytes = recvfrom(udp_fd, buf, 100, 0, (struct sockaddr *)&from_addr, &addr_len);

    buf[numbytes] = '\0';
    printf("Received UDP response: %s\n", buf);

    close(udp_fd);
    return 0;
}
