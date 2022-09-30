#pragma once
#include <string>
#include <iostream>

#include "socket.cpp"
#include "information_subservice.cpp"

#define MAGIC_PORT 4005
#define KEEPALIVE_PORT 4002

using namespace std;

void listenForKeepalives() {
  Socket ptcp_socket;
  int send_res, recv_res;
  string mac_addr = getMacAddress();
  string hostname = getSelfHostname();

  ptcp_socket.setTimeoutOpt();
  ptcp_socket.listenPort(KEEPALIVE_PORT);

  while(manager) {
    // Listen for Keepalive queries sent by the participants
    recv_res = ptcp_socket.receiveMessage(true);
    if (recv_res < 0)
      //cerr << "[P] ERROR recvfrom" << endl;
      continue;

    // Answers the Keepalive query received
    send_res = ptcp_socket.sendMessageToSender(mac_addr + hostname);
    if (send_res < 0)
      cerr << "[P] ERROR sendto" << endl;
  }
  ptcp_socket.closeSocket();
}

void becomeParticipant(){
  manager = false;
}

void magicListener(){
  Socket ptcp_socket;
  int send_res, recv_res;
  string mac_addr = getMacAddress();
  string hostname = getSelfHostname();

  ptcp_socket.setTimeoutOpt();
  ptcp_socket.listenPort(MAGIC_PORT);

  while(manager) {
    // Listen for Keepalive queries sent by the participants
    recv_res = ptcp_socket.receiveMessage(true);
    if (recv_res < 0)
      //cerr << "[P] ERROR recvfrom" << endl;
      continue;
    becomeParticipant();
  }
  ptcp_socket.closeSocket();
}

