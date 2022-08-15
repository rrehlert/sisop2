#pragma once
#include <string>
#include <iostream>
#include <time.h>
#include <map>

#include "socket.cpp"
#include "machine.cpp"
#include "management_subservice.cpp"
#include "information_subservice.cpp"

#define PORT 4000
#define EXIT_PORT 4001
#define MAX_MISSED_CALLS 10
#define MONITORING_PACKET_INTERVAL 1

using namespace std;

void monitorateParticipant(string IP) {
  Socket mng_socket;
  int send_res, recv_res, missed_calls = 0;
  bool awake = true;
  auto machine = MachinesManager::Instance().getMachine(IP);
  string mac_addr = getMacAddress();
  string hostname = getSelfHostname();


	mng_socket.setSendAddr(IP, PORT);

  while(machine->second.isParticipating()) {

    // Send packet looking for participants
    send_res = mng_socket.sendMessage(mac_addr + hostname);
    if (send_res < 0)
      cerr << ("[M] ERROR sendto") << endl;

    recv_res = mng_socket.receiveMessage();
    if (recv_res < 0) {
      missed_calls++;
      // cerr << "[M] Participant " << IP << " didn't answer. Missed calls: " << missed_calls << endl;
      if ((awake == true) && (missed_calls >= MAX_MISSED_CALLS)) {
        // cout << "[M] Participant " << IP << " set as ASLEEP" << endl;
        awake = false;
        machine->second.setAsleep();
        MachinesManager::Instance().setMapChanged(true);
      }
    }
    else {
      // cout << "[M] Participant " << IP << " answered: " << mng_socket.getBuffer() << endl;
      missed_calls = 0;
      if (awake == false) {
        awake = true;
        machine->second.setAwake();
        MachinesManager::Instance().setMapChanged(true);
      }
    }

    sleep(MONITORING_PACKET_INTERVAL);
  }
}

void exitListener(){
  Socket exit_socket;
  int send_res, recv_res;
  string participant_ip;

  exit_socket.listenPort(EXIT_PORT);

    while(true) {
    // Listen for packets sent by the manager to PORT
    recv_res = exit_socket.receiveMessage(true);
    if (recv_res < 0)
      cerr << "[M] ERROR recvfrom" << endl;
    // cout << "[M] Participant (IP " << exit_socket.getSenderIP() << " ) asked: " << exit_socket.getBuffer() << endl;
    participant_ip = exit_socket.getSenderIP();
    MachinesManager::Instance().removeMachine(participant_ip);
    // cout << participant_ip << " Machine exited the service" << endl;

    MachinesManager::Instance().setMapChanged(true);

  }

}
