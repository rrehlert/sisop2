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
		system("clear");
		cout << "Role: [M]" << endl;
    cout << "No participants found yet" << endl;

		// Discovery Subservice
		thread (discoverParticipants).detach();
		
		// Exit Handler Subservice
		thread (exitListener).detach();

		// CLI Subservice
		thread (read_CLI).detach();

		thread (updateInterface).detach();
  }
  else {
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
