#pragma once
#include <string>
#include <iostream>
#include <time.h>
#include <map>
#include <thread>

#include "socket.cpp"
#include "machine.cpp"
#include "management_subservice.cpp"
#include "monitoring_subservice.cpp"

#define PORT 4000
#define BROADCAST_IP "255.255.255.255"

using namespace std;

void discoverParticipants() {
  Socket mng_socket;
  int send_res, recv_res;

  mng_socket.setBroadcastOpt();
	mng_socket.setSendAddr(BROADCAST_IP, PORT);

  while(true) {
    // Send packet looking for participants
    send_res = mng_socket.sendMessage("Are you here?");
    if (send_res < 0)
      cerr << ("[D] ERROR sendto") << endl;

    recv_res = mng_socket.receiveMessage();
    if (recv_res < 0) {
      cerr << "[D] Nobody answered \n" << endl;
    }
    else {
      cout << "[D] Participant answered: " << mng_socket.getBuffer() << endl;

      string IP_addr = mng_socket.getSenderIP();
      string hostname = mng_socket.getSenderHostname();

      // Looking for the machine on the map
      if (MachinesManager::Instance().machineIsKnown(IP_addr)) {
        cout << "[D] Machine is known \n" << endl;
        auto machine = MachinesManager::Instance().getMachine(IP_addr);
        if (machine->second.isParticipating() == false) {
          machine->second.setParticipating(true);
          thread (monitorateParticipant, IP_addr).detach();
        }
      }
      else {
        cout << "[D] Machine is unknown! Adding to the map \n" << endl;
        MachinesManager::Instance().createMachine(hostname, IP_addr);
        thread (monitorateParticipant, IP_addr).detach();
      }
    }

    MachinesManager::Instance().printMachines();

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
