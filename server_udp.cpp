#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

#define PORT 4000

int main(int argc, char *argv[])
{
	int sockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char buf[256];
	char broadcast = '1';

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
		printf("ERROR opening socket");
	setsockopt(sockfd, SOL_SOCKET,SO_BROADCAST, &broadcast, sizeof(broadcast));	
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);    
	 
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
		printf("ERROR on binding");

	clilen = sizeof(struct sockaddr_in);
	
	while (1) {
		/* receive from socket */
		n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *) &cli_addr, &clilen);
		if (n < 0) 
			printf("ERROR on recvfrom");
		printf("Client ip: %s\n", inet_ntoa(cli_addr.sin_addr));
		printf("Received a datagram: %s\n", buf);

		// n = recv(sockfd, buf, 256, 0);
		// if (n<0)
		// 	printf("ERROR on recv");
		// printf("Received a message");
		
		/* send to socket */

		n = sendto(sockfd, "Got your message\n", 17, 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
		if (n  < 0) 
			printf("ERROR on sendto");
		bzero(buf, 256);
	}
	
	close(sockfd);
	return 0;
}
