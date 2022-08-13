#include <string>
#include <iostream>
#include <time.h>
#include <map>
#include <thread>

#include "machine.cpp"
#include "socket.cpp"
#include "discovery_subservice.cpp"

#define PORT 4000
#define BROADCAST_IP "255.255.255.255"

using namespace std;

int main(int argc, char *argv[]) {
  bool manager = false;

	if (argc > 1 && (string)argv[1] == "manager")
    manager = true;

  if (manager == true) {

		// Interface Subservice:
		// - thread mostrando a lista de participantes;
		// - thread esperando input do usuario

		// Discovery Subservice:
		// - thread enviando pacotes em modo broadcast;
		// - (?) thread ouvindo pacotes de exit

		// Monitoring Subservice:
		// - uma thread para cada participante, perguntando se esta acordado

		cout << "Role: [M]" << endl;

		thread (discoverParticipants).detach();
		//-----------> monitorateParticipant() pra cada participante aqui <------------
  }
  else {

		// Interface Subservice:
		// - thread esperando input do usuario

		// Discovery Subservice:
		// - thread ouvindo pacotes de descoberta

		// Monitoring Subservice:
		// - thread ouvindo pacotes de status

		cout << "Role: [P]" << endl;

		thread (listenForDiscovery).detach();
			// e se existir um unico metodo que responde `sim` pra 
			// discovery e monitoring, ou seja, pra tudo o que o ptcp escutar?
			// onde colocar esse metodo?
		
  }

	while(true) {};

	return 0;
}
