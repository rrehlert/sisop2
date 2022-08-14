#pragma once
#include <iostream>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <net/if.h>
#include <stdlib.h>

#include "socket.cpp"

#define PORT 4000
#define EXIT_PORT 4001
#define MAGIC_PORT 9

using namespace std;

string manager_ip;

string exec(string command) {
   char buffer[128];
   string result = "";

   // Open pipe to file
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return "popen failed!";
   }

   // read till end of process:
   while (!feof(pipe)) {

      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }

   pclose(pipe);
   return result;
}

string getMacAddress(){
  string command; 
  string mac;

  //Execute terminal command to get ethernet informantion
  command = exec("ip a | grep -A1 enp");
  if(command.length() == 0)
    command = exec("ip a | grep -A1 eth0");
  int pos = command.find("link/ether");

  //Scan for mac address
  mac = command.substr(pos+11, 17);
  return mac;
}

void listenForServicePackets() {
  Socket ptcp_socket;
  int send_res, recv_res;
  string mac_addr = getMacAddress();

  ptcp_socket.listenPort(PORT);

  while(true) {
    // Listen for packets sent by the manager to PORT
    recv_res = ptcp_socket.receiveMessage(true);
    if (recv_res < 0)
      cerr << "[P] ERROR recvfrom" << endl;
    cout << "[P] Manager (IP " << ptcp_socket.getSenderIP() << " ) asked: " << ptcp_socket.getBuffer() << endl;
    manager_ip = ptcp_socket.getSenderIP();
    // Answers the packet received
    send_res = ptcp_socket.sendMessageToSender(mac_addr);
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