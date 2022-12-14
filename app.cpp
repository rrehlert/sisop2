#include <string>
#include <iostream>
#include <thread>

#include "discovery_subservice.cpp"
#include "participant_subservice.cpp"
#include "manager_subservice.cpp"
#include "interface_subservice.cpp"
#include "replication_subservice.cpp"
#include "election_subservice.cpp"

using namespace std;

bool manager = false;

int main(int argc, char *argv[]) {

  // redirect log messages to a file
  freopen("log.txt", "w", stderr);

  system("clear");
  cout << "Role: Participant" << endl;
  cout << "No manager found yet" << endl;

  // Participant Subservice
  thread (listenForServicePackets).detach();
  thread (monitorateManagerStatus).detach();

  // Replication Subservice
  thread (listenForReplicationPackets).detach();

  // Election Subservice
  thread (listenForElectionPackets).detach();

  // Interface Subservice
  thread (read_CLI).detach();
  thread (updateInterface).detach();
// while(!manager) {}
  // log
  // cerr << "Becoming manager" << endl;
  // system("clear");
  // cout << "Role: Manager" << endl;
  // cout << "No participants found yet" << endl;

  // Manager Subservice
  thread (listenForKeepalives).detach();

  // Discovery Subservice
  thread (discoverParticipants).detach();

  // Listen when manager is awaken
  thread (magicListener).detach();

  // Exit Handler Subservice
  thread (exitListener).detach();

  // Interface Subservice
  // thread (read_CLI).detach();
  // thread (updateManagerInterface).detach();
  while(true) {};

	return 0;
}
