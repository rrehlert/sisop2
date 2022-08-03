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
#define BROADCAST_IP "255.255.255.255"

int main(int argc, char *argv[])
{
  int mng_sock_fd, ptcp_sock_fd, send_res, recv_res;
	unsigned int length;
	struct sockaddr_in ptcp_addr, mng_addr, broadcast_addr, from;
	struct hostent *server;
	char buffer[256];
	char input[256];
	int broadcastPermission = 1;
  bool manager = false;

	if (argc > 0 && argv[0] == "manager") 
    manager = true;
	

  if (manager) {
    if ((mng_sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
      printf("ERROR opening manager socket");
    if (setsockopt(mng_sock_fd, SOL_SOCKET,SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission)) < 0){
      fprintf(stderr, "setsockopt error");
      exit(1);
    }
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(PORT);
    broadcast_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP);
    bzero(&(broadcast_addr.sin_zero), 8);
  }
  else {
    if ((ptcp_sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
      printf("ERROR opening participant socket");
    ptcp_addr.sin_family = AF_INET;
    ptcp_addr.sin_port = htons(PORT);
    ptcp_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(ptcp_addr.sin_zero), 8);
  }

	while(1){
    if (manager) {
	  	// Envia pacote a procura de participantes acordados
      send_res = sendto(mng_sock_fd, "Are you awake?\n", 15, 0, (const struct sockaddr *) &ptcp_addr, sizeof(struct sockaddr_in));
		  if (send_res < 0)
			  printf("ERROR manager sendto");
      
      // Recebe resposta dos participantes
      // TODO: receber as respostas (que podem vir ao mesmo tempo) em ordem de chegada
      // NENHUMA mensagem deve ser perdida
      recv_res = recvfrom(mng_sock_fd, buffer, 256, 0, (struct sockaddr *) &ptcp_addr, &length);
      if (recv_res < 0)
        printf("ERROR recvfrom");
    }
    else {
      // Espera mensagem do manager
		  recv_res = recvfrom(mng_sock_fd, buffer, 256, 0, (struct sockaddr *) &mng_addr, &length);
      if (recv_res < 0)
        printf("ERROR participant recvfrom");

      // Responde ao manager
      send_res = sendto(mng_sock_fd, "Yes", 15, 0, (const struct sockaddr *) &mng_addr, sizeof(struct sockaddr_in));
		  if (send_res < 0)
			  printf("ERROR manager sendto");
      
    }
		
		length = sizeof(struct sockaddr_in);
		printf("Got an ack: %s\n", buffer);
		printf("Server ip: %s\n", inet_ntoa(from.sin_addr));
		printf("Server PORT: %d\n", ntohs(from.sin_port));

		server = gethostbyaddr(&(from.sin_addr), sizeof(from.sin_addr), AF_INET);
		//server = gethostbyname(inet_ntoa(from.sin_addr));
		if (server == NULL)
			printf("Couldn't get host name, using: %s\n", inet_ntoa(from.sin_addr));
		else
			printf("Server NAME: %s\n", server->h_name);

		// printf("Server ip: %s\n", inet_ntoa(ptcp_addr.sin_addr));
		// printf("Server PORT: %d\n", ntohs(ptcp_addr.sin_port));
		sleep(1);
		n = sendto(mng_sock_fd, "Hello\n", 6, 0, (const struct sockaddr *) &from, sizeof(struct sockaddr_in));
		if (n < 0) 
			printf("ERROR sendto");
		else
			printf("message sent\n");

		n = recvfrom(mng_sock_fd, buffer, 256, 0, (struct sockaddr *) &from, &length);
		if (n < 0)
			printf("ERROR recvfrom");
		printf("buffer: %s\n", buffer);
		sleep(2);
	}
	close(mng_sock_fd);
	return 0;
}