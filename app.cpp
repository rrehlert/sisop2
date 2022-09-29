#include <string>
#include <iostream>
#include <thread>

#include "discovery_subservice.cpp"
#include "participant_subservice.cpp"
#include "manager_subservice.cpp"
#include "interface_subservice.cpp"
#include "replication_subservice.cpp"

using namespace std;

bool manager = false;

int main(int argc, char *argv[]) {

//	if (argc > 1 && (string)argv[1] == "manager")
//    manager = true;

		system("clear");
  	cout << "Role: Participant" << endl;
  	cout << "No manager found yet" << endl;

		// Participant Subservice
		thread (listenForServicePackets).detach();
		thread (monitorateManagerStatus).detach();

		// Replication Subservice
		thread (listenForReplicationPackets).detach();

		// CLI Subservice
		thread (read_CLI).detach();
		thread (updateParticipantInterface).detach();

	cout << "EXIT PARTICIPANT" << endl;
	while(!manager) {}
	if (manager == true) {
			cout << "ENTERING MANAGER" << endl;
			system("clear");
			cout << "Role: Manager" << endl;
			cout << "No participants found yet" << endl;

			// Manager Subservice
			thread (listenForKeepalives).detach();

			// Discovery Subservice
			thread (discoverParticipants).detach();

			// Exit Handler Subservice
			thread (exitListener).detach();

			// CLI Subservice
			//thread (read_CLI).detach();
			thread (updateManagerInterface).detach();
  		}
	while(true){}
	return 0;
}
