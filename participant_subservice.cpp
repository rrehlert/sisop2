#pragma once
#include <iostream>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <net/if.h>
#include <stdlib.h>

#include "socket.cpp"
#include "information_subservice.cpp"

#define PORT 4000
#define EXIT_PORT 4001
#define MAGIC_PORT 9

using namespace std;

string manager_ip, manager_mac, manager_hostname;

void listenForServicePackets() {
  Socket ptcp_socket;
  int send_res, recv_res;
  string mac_addr = getMacAddress();
  string hostname = getSelfHostname();

  ptcp_socket.listenPort(PORT);

  while(true) {
    // Listen for packets sent by the manager to PORT
    recv_res = ptcp_socket.receiveMessage(true);
    if (recv_res < 0)
      cerr << "[P] ERROR recvfrom" << endl;
    // cout << "[P] Manager (IP " << ptcp_socket.getSenderIP() << " ) asked: " << ptcp_socket.getBuffer() << endl;
    
    //Get manager infos
    manager_ip = ptcp_socket.getSenderIP();
    string buffer = ptcp_socket.getBuffer();
    string manager_mac =  buffer.substr(0,17);
    string manager_hostname = buffer.substr(17);
    system("clear");
    cout << "Role: [P]" << endl;
    cout << "Latest manager info:" << endl;
    cout.width(25); cout << left << "hostname";
		cout.width(15); cout << left << "IP";
		cout.width(21); cout << left << "Mac" << endl;
    cout.width(25); cout << left << manager_hostname;
		cout.width(15); cout << left << manager_ip;
		cout.width(21); cout << left << manager_mac << endl;


    // Answers the packet received
    send_res = ptcp_socket.sendMessageToSender(mac_addr + hostname);
    if (send_res < 0)
      cerr << "[P] ERROR sendto" << endl;
  }
}

void sendExitPacket(){
  Socket exit_socket;
  exit_socket.setSendAddr(manager_ip, EXIT_PORT);
  //Send exit message to specified exit port
  int exit_message = exit_socket.sendMessage("sleep service exit");
}