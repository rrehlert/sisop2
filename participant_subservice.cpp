#pragma once
#include <iostream>

#include "socket.cpp"

#define PORT 4000

using namespace std;

void listenForServicePackets() {
  Socket ptcp_socket;
  int send_res, recv_res;

  ptcp_socket.listenPort(PORT);

  while(true) {
    // Listen for packets sent by the manager to PORT
    recv_res = ptcp_socket.receiveMessage(true);
    if (recv_res < 0)
      cerr << "[P] ERROR recvfrom" << endl;
    cout << "[P] Manager (IP " << ptcp_socket.getSenderIP() << ") asked: " << ptcp_socket.getBuffer() << endl;

    // Answers the packet received
    send_res = ptcp_socket.sendMessageToSender("Yes");
    if (send_res < 0)
      cerr << "[P] ERROR sendto" << endl;
  }
}
