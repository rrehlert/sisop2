#include <string>
#include <iostream>
#include <time.h>
#include <map>

#include "machine.cpp"
#include "socket.cpp"

#define PORT 4000
#define BROADCAST_IP "255.255.255.255"

using namespace std;

void discoverParticipants() {
  Socket mng_socket;
  int send_res, recv_res;
  map<string, Machine> machines;

  mng_socket.setBroadcastOpt();
	mng_socket.setSendAddr(BROADCAST_IP, PORT);

  while(true) {
    // Send packet looking for participants
    send_res = mng_socket.sendMessage("Are you here?");
    if (send_res < 0)
      cerr << ("[M] ERROR sendto") << endl;

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
}

void listenForDiscovery() {
  Socket ptcp_socket;
  int send_res, recv_res;

  ptcp_socket.listenPort(PORT);

  while(true) {
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
