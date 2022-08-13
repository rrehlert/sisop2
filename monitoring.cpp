#pragma once
#include <string>
#include <iostream>
#include <time.h>
#include <map>

#include "socket.cpp"
#include "machine.cpp"
#include "machines_management.cpp"

#define PORT 4000
#define MAX_MISSED_CALLS 10

using namespace std;

void monitorateParticipant(string IP) {
  Socket mng_socket;
  int send_res, recv_res, missed_calls = 0;
  auto machine = MachinesManager::Instance().getMachine(IP);

	mng_socket.setSendAddr(IP, PORT);

  while(true) {

    // Send packet looking for participants
    send_res = mng_socket.sendMessage("Are you awake?");
    if (send_res < 0)
      cerr << ("[M] ERROR sendto") << endl;

    recv_res = mng_socket.receiveMessage();
    if (recv_res < 0) {
      cerr << "[M] Participant " << IP << " didn't answer \n" << endl;
      missed_calls++;
      if (missed_calls >= MAX_MISSED_CALLS) {
        machine->second.setAsleep();
      }
    }
    else {
      cout << "[M] Participant " << IP << " answered: " << mng_socket.getBuffer() << endl;
      if (machine->second.isAsleep()) {
        machine->second.setAwake();
      }
    }

    sleep(1);
  }
}

void listenForMonitoring() {
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
