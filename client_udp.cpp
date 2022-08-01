#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define PORT 4000

int main(int argc, char *argv[])
{
    int sockfd, n;
	unsigned int length;
	struct sockaddr_in serv_addr, from;
	struct hostent *server;
	char buffer[256];
	char input[256];
	int broadcastPermission = 1;
	char *broadcastIP;

	// if (argc < 2) {
	// 	fprintf(stderr, "usage %s hostname\n", argv[0]);
	// 	exit(0);

	// }
	//char *bcserver = "127.0.0.255";
	broadcastIP = "255.255.255.255";
	printf(argv[1]);

	// server = gethostbyname(bcserver);
	// if (server == NULL) {
    //     fprintf(stderr,"ERROR, no such host\n");
    //     exit(0);
    // }	
	
	// BroadcastAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	// BroadcastAddr->SetBroadcastAddress();
	// BroadcastAddr->SetPort(UDPListenPort);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("ERROR opening socket");
	if (setsockopt(sockfd, SOL_SOCKET,SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission)) < 0){
		fprintf(stderr, "setsockopt error");
		exit(1);
	}
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(PORT);    
	serv_addr.sin_addr.s_addr = inet_addr(broadcastIP);
	bzero(&(serv_addr.sin_zero), 8);  

	// printf("Enter the message: ");
	// bzero(buffer, 256);
	// fgets(buffer, 256, stdin);
	// bzero(input,256);
	// strcpy(buffer,input);
	// printf("%s\n", input);
	//n = send(sockfd,buffer, strlen(buffer), 0);
	while(1){
		n = sendto(sockfd, "Are you awake?\n", 15, 0, (const struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
		if (n < 0) 
			printf("ERROR sendto");
		
		length = sizeof(struct sockaddr_in);
		n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *) &from, &length);
		if (n < 0)
			printf("ERROR recvfrom");
		printf("Got an ack: %s\n", buffer);
		printf("Server ip: %s\n", inet_ntoa(from.sin_addr));
		printf("Server PORT: %d\n", ntohs(from.sin_port));

		server = gethostbyaddr(&(from.sin_addr), sizeof(from.sin_addr), AF_INET);
		//server = gethostbyname(inet_ntoa(from.sin_addr));
		if (server == NULL)
			printf("Couldn't get host name, using: %s\n", inet_ntoa(from.sin_addr));
		else
			printf("Server NAME: %s\n", server->h_name);

		// printf("Server ip: %s\n", inet_ntoa(serv_addr.sin_addr));
		// printf("Server PORT: %d\n", ntohs(serv_addr.sin_port));
		sleep(1);
		n = sendto(sockfd, "Hello\n", 6, 0, (const struct sockaddr *) &from, sizeof(struct sockaddr_in));
		if (n < 0) 
			printf("ERROR sendto");

		n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *) &from, &length);
		if (n < 0)
			printf("ERROR recvfrom");
		printf("Second message sent\n");
		sleep(2);
	}
	close(sockfd);
	return 0;
}