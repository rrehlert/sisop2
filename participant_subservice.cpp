#pragma once
#include <iostream>

#include "socket.cpp"

#define PORT 4000
#define EXIT_PORT 4001

using namespace std;

string manager_ip;

void listenForServicePackets() {
  Socket ptcp_socket;
  int send_res, recv_res;
   
  ptcp_socket.listenPort(PORT);

  while(true) {
    // Listen for packets sent by the manager to PORT
    recv_res = ptcp_socket.receiveMessage(true);
    if (recv_res < 0)
      cerr << "[P] ERROR recvfrom" << endl;
    cout << "[P] Manager (IP " << ptcp_socket.getSenderIP() << " ) asked: " << ptcp_socket.getBuffer() << endl;
    manager_ip = ptcp_socket.getSenderIP();
    // Answers the packet received
    send_res = ptcp_socket.sendMessageToSender("Yes");
    cout << "Message Sent" << endl;
    if (send_res < 0)
      cerr << "[P] ERROR sendto" << endl;
  }
}

void sendExitPacket(){
  Socket exit_socket;
  exit_socket.setSendAddr(manager_ip, EXIT_PORT);

  int exit_message = exit_socket.sendMessage("sleep service exit");

}