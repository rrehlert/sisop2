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
#include "information_subservice.cpp"

#define PORT 4000
#define BROADCAST_IP "255.255.255.255"
#define DISCOVERY_BEACON_INTERVAL 2

using namespace std;

void discoverParticipants() {
  Socket mng_socket;
  int send_res, recv_res;

  string mac_addr = getMacAddress();
  string hostname = getSelfHostname();

  mng_socket.setBroadcastOpt();
	mng_socket.setSendAddr(BROADCAST_IP, PORT);

  while(true) {
    // Send packet looking for participants
    send_res = mng_socket.sendMessage(mac_addr + hostname);
    if (send_res < 0)
      cerr << ("[D] ERROR sendto") << endl;

    while (mng_socket.receiveMessage() > 0) {
      // cout << "[D] Participant answered: " << mng_socket.getBuffer() << endl;

      string IP_addr = mng_socket.getSenderIP();
      //string hostname = mng_socket.getSenderHostname();
      string buffer = mng_socket.getBuffer();
      string mac_addr =  buffer.substr(0,17);
      string hostname = buffer.substr(17);
      if (hostname.compare("") == 0)
        hostname = IP_addr;

      // Looking for the machine on the map
      if (MachinesManager::Instance().machineIsKnown(IP_addr)) {
        // cout << "[D] Machine is known \n" << endl;

        auto machine = MachinesManager::Instance().getMachine(IP_addr);
        // Set machine to be monitorated again
        if (machine->second.isParticipating() == false) {
          if (machine->second.getCount() >= 2){
            // cout << "[D] Machine " << IP_addr << " is being monitorated again" << endl;
            machine->second.setParticipating(true);
            MachinesManager::Instance().setMapChanged(true);
            // Monitoring Subservice
            thread (monitorateParticipant, IP_addr).detach();
          }
          else{
            machine->second.incrementCount();
          }
        }
      }
      else {
        // cout << "[D] Machine is unknown! Adding to the map \n" << endl;
        MachinesManager::Instance().createMachine(IP_addr, mac_addr, hostname);
        MachinesManager::Instance().setMapChanged(true);

        // Monitoring Subservice
        thread (monitorateParticipant, IP_addr).detach();
      }
    }

    sleep(DISCOVERY_BEACON_INTERVAL);
  }
}
