#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MYPORT 3500 /* TCP-portti asiakkaan pyynnölle */
#define UDPPORT 4950 /* UDP-portti vastauksen lähetykselle */
int main(void)
{
 int sockfd, new_fd, udp_fd, sin_size, numbytes, i;
 struct sockaddr_in my_addr;
 struct sockaddr_in their_addr;
 struct sockaddr_in client_udp_addr;
 char buf[101];
 /* Luodaan TCP-soketti asiakkaan yhteydenottoa varten */
 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 my_addr.sin_family = AF_INET;
 my_addr.sin_port = htons(MYPORT);
 my_addr.sin_addr.s_addr = INADDR_ANY;
 /* Sidotaan TCP-soketti kuunteluporttiin */
 bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
 /* Asetetaan palvelin kuuntelutilaan */
 listen(sockfd, 10);
 /* Luodaan erillinen UDP-soketti vastauksen lähetystä varten */
 udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
 while(1)
 {
 sin_size = sizeof(struct sockaddr_in);

 /* Hyväksytään saapuva TCP-yhteys asiakkaalta */
 new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
 printf("From %s\n", inet_ntoa(their_addr.sin_addr));
 /* Luetaan asiakkaan lähettämä tekstijono TCP-yhteyden yli */
 numbytes = recv(new_fd, buf, 100, 0);
 if (numbytes > 0) {
 buf[numbytes] = '\0';
 printf("Received TCP text: %s\n", buf);
 /* Muunnetaan vastaanotettu teksti isoksi kirjaimiksi */
 for (i = 0; i < numbytes; i++) {
 buf[i] = toupper((unsigned char)buf[i]);
 }
 }
 /* Suljetaan TCP-soketti tiedonsiirron jälkeen */
 close(new_fd);
 /* Valmistellaan osoitetietue UDP-vastausta varten asiakkaan IP-osoitteeseen */
 client_udp_addr.sin_family = AF_INET;
 client_udp_addr.sin_port = htons(UDPPORT);
 /* UDP-vastaus lähetetään asiakkaan TCP-yhteydestä saadulla IP-osoitteella */
 client_udp_addr.sin_addr = their_addr.sin_addr;
 /* Lähetetään muunnettu isokirjaiminen teksti asiakkaalle UDP-soketilla */
 sendto(udp_fd, buf, strlen(buf), 0,
 (struct sockaddr *)&client_udp_addr, sizeof(struct sockaddr));
 printf("Sent UDP response: %s\n", buf);
 }
 close(sockfd);
 close(udp_fd);
 return 0;
}