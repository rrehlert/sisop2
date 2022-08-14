#include <string>
#include <iostream>
#include <time.h>
#include <map>
#include <thread>

#include "machine.cpp"
#include "socket.cpp"
#include "discovery_subservice.cpp"
#include "participant_subservice.cpp"
#include "interface_subservice.cpp"
#include "management_subservice.cpp"

#define PORT 4000
#define BROADCAST_IP "255.255.255.255"

using namespace std;

bool manager = false;

int main(int argc, char *argv[]) {
  

	if (argc > 1 && (string)argv[1] == "manager")
    manager = true;

  if (manager == true) {

		// Interface Subservice:
		// [ ] thread mostrando a lista de participantes;
		// [ ] thread esperando input do usuario

		// Discovery Subservice:
		// [x] thread enviando pacotes em modo broadcast;
		// [ ] (?) thread ouvindo pacotes de exit

		// Monitoring Subservice:
		// [x] uma thread para cada participante, perguntando se esta acordado
		system("clear");
		cout << "Role: [M]" << endl;
    cout << "No participants found yet" << endl;

		// Discovery Subservice
		thread (discoverParticipants).detach();
		
		// Exit Handler Subservice
		thread (exitListener).detach();

		// CLI Subservice
		thread (read_CLI).detach();
  }
  else {

		// Interface Subservice:
		// [ ] thread esperando input do usuario

		// Discovery/Monitoring Subservice:
		// [x] thread ouvindo pacotes de descoberta e pacotes de status

		system("clear");
  	cout << "Role: [P]" << endl;
  	cout << "No manager found yet" << endl;

		// Participant Subservice
		thread (listenForServicePackets).detach();

		// CLI Subservice
		thread (read_CLI).detach();
  }

	while(true) {};

	return 0;
}
