#pragma once
#include <string>
#include <iostream>

#include "socket.cpp"
#include "information_subservice.cpp"

#define KEEPALIVE_PORT 4002

using namespace std;

void listenForKeepalives() {
  Socket ptcp_socket;
  int send_res, recv_res;
  string mac_addr = getMacAddress();
  string hostname = getSelfHostname();

  ptcp_socket.listenPort(KEEPALIVE_PORT);

  while(true) {
    // Listen for Keepalive queries sent by the participants
    recv_res = ptcp_socket.receiveMessage(true);
    if (recv_res < 0)
      cerr << "[P] ERROR recvfrom" << endl;

    // Answers the Keepalive query received
    send_res = ptcp_socket.sendMessageToSender(mac_addr + hostname);
    if (send_res < 0)
      cerr << "[P] ERROR sendto" << endl;
  }
}
