#pragma once
#include <string>
#include <iostream>

#include "socket.cpp"
#include "information_subservice.cpp"

#define MAGIC_PORT 4005
#define KEEPALIVE_PORT 4002

using namespace std;
extern bool manager;

void listenForKeepalives() {
  Socket ptcp_socket;
  int send_res, recv_res;
  string mac_addr = getMacAddress();
  string hostname = getSelfHostname();

  cerr << "Entrando na listenForKeepalives" << endl;

  ptcp_socket.setTimeoutOpt();
  ptcp_socket.listenPort(KEEPALIVE_PORT);

  while(true) {
    if (!manager) {
      continue;
    }
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

  cerr << "Entrando na magicListener" << endl;

  // ptcp_socket.setTimeoutOpt();
  ptcp_socket.listenPort(MAGIC_PORT);

  while(true) {
    if (!manager) {
      continue;
    }
    // Listen for Keepalive queries sent by the participants
    recv_res = ptcp_socket.receiveMessage(true);
    if (recv_res < 0)
      //cerr << "[P] ERROR recvfrom" << endl;
      continue;
    string msg = ptcp_socket.getBuffer();
    string mac = msg.substr(0, 17);
    if (mac == mac_addr) {
      becomeParticipant();
    }
  }
  ptcp_socket.closeSocket();
}

