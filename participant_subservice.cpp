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
#define KEEPALIVE_PORT 4002
#define MAX_MISSED_KEEPALIVES 3
#define MAX_MISSED_DISCOVERY 3
#define KEEPALIVE_INTERVAL 1

using namespace std;

string manager_ip = "", manager_mac = "", manager_hostname = "";
bool manager_changed = false;
extern bool manager;

void listenForServicePackets() {
  Socket ptcp_socket;
  int send_res, recv_res;
  string mac_addr = getMacAddress();
  string hostname = getSelfHostname();

  ptcp_socket.setTimeoutOpt();
  ptcp_socket.listenPort(PORT);

  while(!manager) {
    // if (manager_ip == "")
    //   continue;
    // Listen for packets sent by the manager to PORT

    recv_res = ptcp_socket.receiveMessage(true);
    if (recv_res < 0){
      //cerr << "[P] ERROR recvfrom" << endl;
      //cout << recv_res << endl;
      continue;
    }
    // cout << "[P] Manager (IP " << ptcp_socket.getSenderIP() << " ) asked: " << ptcp_socket.getBuffer() << endl;

    // Get manager infos
    string received_ip = ptcp_socket.getSenderIP();
    string buffer = ptcp_socket.getBuffer();
    string received_mac =  buffer.substr(0,17);
    string received_hostname = buffer.substr(17);

    if (manager_ip != received_ip || manager_mac != received_mac || manager_hostname != received_hostname) {
      manager_ip = received_ip;
      manager_mac = received_mac;
      manager_hostname = received_hostname;
      manager_changed = true;
    }

    // Answers the packet received
    send_res = ptcp_socket.sendMessageToSender(mac_addr + hostname);
    if (send_res < 0)
      cerr << "[P] ERROR sendto" << endl;
  }
  ptcp_socket.closeSocket();
  //cout << "Exiting Thread 1";
}

void startManagerElection() {
  cout << "[P] I'm starting a Manager election" << endl;
}

void monitorateManagerStatus() {
  Socket ptcp_socket;
  int send_res, recv_res, missed_keepalives = 0, manager_discovery_count = 0;
  string mac_addr = getMacAddress();
  string hostname = getSelfHostname();

  // Wait for the discovery of a manager
  while(manager_ip == "" && !manager) {
    // cout << "[P] Manager not known yet" << endl;
    // manager_discovery_count++ // IDEA
    // if(manager_discovery_count >= MAX_MISSED_DISCOVERY) // IDEA
    // startManagerElection(); // IDEA
    //
    sleep(1);
  }

  ptcp_socket.setSendAddr(manager_ip, KEEPALIVE_PORT);
  while (!manager){
    bool keep_monitoring = true;
    while(keep_monitoring && !manager) {

      // Send packet querying the manager's status
      send_res = ptcp_socket.sendMessage(mac_addr + hostname);
      if (send_res < 0)
        cerr << ("[P] ERROR sendto") << endl;

      recv_res = ptcp_socket.receiveMessage();
      if (recv_res < 0) {
        missed_keepalives++;
        cerr << "[P] Manager didn't answer. Missed keepalives: " << missed_keepalives << endl;
        if (missed_keepalives >= MAX_MISSED_KEEPALIVES) {
          // cout << "[P] Starting a Manager election" << endl;
          startManagerElection();
          // if (newManager == true) {becomeManager()} // IDEA
          keep_monitoring = false;
        }
      }
      else {
        // cout << "[P] Manager " << IP << " answered: " << ptcp_socket.getBuffer() << endl;
        missed_keepalives = 0;
      }

      sleep(KEEPALIVE_INTERVAL);
    }
  }
  ptcp_socket.closeSocket();
  //cout << "Exiting Thread 2";
}

void sendExitPacket(){
  Socket exit_socket;
  exit_socket.setSendAddr(manager_ip, EXIT_PORT);
  //Send exit message to specified exit port
  int exit_message = exit_socket.sendMessage("sleep service exit");
}

void becomeManager(){
  manager = true;
}