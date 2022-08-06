#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <time.h>
#include <map>

#include "machine.cpp"

#define PORT 4000
#define BROADCAST_IP "255.255.255.255"

using namespace std;

int main(int argc, char *argv[]) {
  int mng_sock_fd, ptcp_sock_fd, send_res, recv_res;
	struct sockaddr_in ptcp_addr, mng_addr, broadcast_addr;
	struct hostent *host_struct;
	unsigned int length = sizeof(struct sockaddr_in);
	char buffer[256];
	int broadcastPermission = 1;
  bool manager = false;
	map<string, Machine> machines;

	if (argc > 1 && (string)argv[1] == "manager") {
    manager = true;
		cout << "Role: [M]" << endl;
	}

  if (manager == true) {
    if ((mng_sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
      cerr << "[M] ERROR opening socket" << endl;
    if (setsockopt(mng_sock_fd, SOL_SOCKET, SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission)) < 0){
      cerr << "[M] setsockopt error" << endl;
      exit(1);
    }
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(PORT);
    broadcast_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP);
  }
  else {
		cout << "Role: [P]" << endl;
    if ((ptcp_sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
      cerr << "[P] ERROR opening socket" << endl;

    ptcp_addr.sin_family = AF_INET;
    ptcp_addr.sin_port = htons(PORT);
    ptcp_addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(ptcp_sock_fd, (struct sockaddr *) &ptcp_addr, sizeof(struct sockaddr)) < 0)
			cerr << "[P] ERROR on binding" << endl;
  }

	while(true) {
    if (manager == true) {

			// Aumenta o counter de chamadas de todas Machines
			for (auto it = machines.begin(); it != machines.end(); ++it)
					it->second.increaseMissedCalls();

	  	// Envia pacote a procura de participantes acordados
      send_res = sendto(mng_sock_fd, "Are you awake?\n", 15, 0, (const struct sockaddr *) &broadcast_addr, sizeof(struct sockaddr_in));
		  if (send_res < 0)
			  cerr << ("[M] ERROR sendto") << endl;

      // Recebe resposta dos participantes
      // TODO: receber as respostas (que podem vir ao mesmo tempo) em ordem de chegada
      // NENHUMA mensagem deve ser perdida
      recv_res = recvfrom(mng_sock_fd, buffer, 256, MSG_DONTWAIT, (struct sockaddr *) &ptcp_addr, &length);
      if (recv_res < 0) {
        cerr << "[M] Nobody answered" << endl;
			}
			else {
				cout << "[M] Participant answered: " << buffer;

				string IP_addr = inet_ntoa(ptcp_addr.sin_addr);
				string hostname;

				host_struct = gethostbyaddr(&(ptcp_addr.sin_addr), sizeof(ptcp_addr.sin_addr), AF_INET);
				if (host_struct == NULL) {
					cerr << "[M] Couldn't get host name, using IP" << endl;
					hostname = IP_addr;
				}
				else {
					hostname = host_struct->h_name;
				}

				// Looking for the machine on the map
				if ( machines.count(IP_addr) > 0) {
					cout << "Machine is known \n" << endl;
					auto it = machines.find(IP_addr);
					it->second.resetMissedCalls();
					it->second.setAwake();
				}
				else {
					cout << "Machine is unknown! Adding to the map \n" << endl;
					Machine mach(IP_addr, hostname);
					machines[IP_addr] = mach;
				}

			}

			cout << "** Machines Map **" << endl;
			for (auto it = machines.begin(); it != machines.end(); ++it) {
				cout << it->first << " => " << endl;
				it->second.print();
				cout << '\n';
			}

			sleep(3);
    }
    else {
      // Espera mensagem do manager
		  recv_res = recvfrom(ptcp_sock_fd, buffer, 256, 0, (struct sockaddr *) &mng_addr, &length);
      if (recv_res < 0)
        cerr << "[P] ERROR recvfrom" << endl;
			buffer[recv_res] = '\0';
			cout << "[P] Manager (IP " << inet_ntoa(mng_addr.sin_addr) << ") asked: " << buffer << endl;

      // Responde ao manager
      send_res = sendto(ptcp_sock_fd, "Yes\n", 4, 0, (const struct sockaddr *) &mng_addr, sizeof(struct sockaddr_in));
		  if (send_res < 0)
			  cerr << "[P] ERROR sendto" << endl;

			sleep(1);
    }
	}

	close(mng_sock_fd);
	close(ptcp_sock_fd);
	return 0;
}
