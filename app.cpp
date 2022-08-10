#include <string>
#include <iostream>
#include <time.h>
#include <map>

#include "machine.cpp"
#include "socket.cpp"

#define PORT 4000
#define BROADCAST_IP "255.255.255.255"

using namespace std;

int main(int argc, char *argv[]) {
	Socket mng_socket, ptcp_socket;
  int send_res, recv_res;
  bool manager = false;
	map<string, Machine> machines;

	if (argc > 1 && (string)argv[1] == "manager")
    manager = true;

  if (manager == true) {
		cout << "Role: [M]" << endl;

		mng_socket.setBroadcastOpt();
		mng_socket.setSendAddr(BROADCAST_IP, PORT);
  }
  else {
		cout << "Role: [P]" << endl;

		ptcp_socket.listenPort(PORT);
  }

	while(true) {
    if (manager == true) {

			// Aumenta o counter de chamadas de todas Machines
			for (auto it = machines.begin(); it != machines.end(); ++it)
					it->second.increaseMissedCalls();

	  	// Envia pacote a procura de participantes acordados
			send_res = mng_socket.sendMessage("Are you awake?");
		  if (send_res < 0)
			  cerr << ("[M] ERROR sendto") << endl;

      // Recebe resposta dos participantes
			// DUVIDA: por que nao precisa chamar bind antes de receber mensagens aqui?
      // TODO: receber respostas (que podem vir ao mesmo tempo) em ordem de chegada; NENHUMA resposta deve ser perdida
			recv_res = mng_socket.receiveMessage();
      if (recv_res < 0) {
        cerr << "[M] Nobody answered \n" << endl;
			}
			else {
				cout << "[M] Participant answered: " << mng_socket.getBuffer() << endl;

				string IP_addr = mng_socket.getSenderIP();
				string hostname = mng_socket.getSenderHostname();

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
			recv_res = ptcp_socket.receiveMessage(true);
      if (recv_res < 0)
        cerr << "[P] ERROR recvfrom" << endl;
			cout << "[P] Manager (IP " << ptcp_socket.getSenderIP() << ") asked: " << ptcp_socket.getBuffer() << endl;

      // Responde ao manager
			send_res = ptcp_socket.sendMessageToSender("Yes");
		  if (send_res < 0)
			  cerr << "[P] ERROR sendto" << endl;
    }
	}

	return 0;
}
