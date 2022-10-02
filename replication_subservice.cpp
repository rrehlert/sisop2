#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <time.h>

#include "socket.cpp"
#include "management_subservice.cpp"
#include "information_subservice.cpp"

#define REPLICATION_PORT 4003

using namespace std;

extern bool manager;

// Replicates the entire Participants map for the new participant `IP_addr`.
// Message prefix: [IR]
void initialReplicationFor(string IP_addr) {
  Socket mng_socket;
  int send_res, recv_res;

  mng_socket.setSendAddr(IP_addr, REPLICATION_PORT);

  // Get a vector with all machines to acquire the current info known to the manager.
  auto machines_vec = MachinesManager::Instance().getVectorOfMachines();

  int vec_size = machines_vec.size();
  int machines_count = 0;

  for (auto it = machines_vec.begin(); it != machines_vec.end(); ++it) {
    string mach_count_s = "[" + to_string(++machines_count) + "/" + to_string(vec_size) + "]";
    string machine_IP = it->getIP();
    string mac_addr = it->getMac();
    string hostname = it->getHostname();
    string status = to_string(it->getStatus());
    string participating = it->isParticipating() ? "1" : "0";
    string id = to_string(it->getID());

    // New Participant packet
    string msg = "[IR]" + mach_count_s;
    msg += machine_IP + '|' + mac_addr + '|' + hostname + '|' + status + '|' + participating + '|' + id;
    send_res = mng_socket.sendMessage(msg);
    if (send_res < 0)
      cerr << ("[R] ERROR sendto") << endl;

    recv_res = mng_socket.receiveMessage();
  }
}

// Replicates a new Participant (described by the 3 arguments) to all participants
// Message prefix: [NP]
void replicateNewParticipant(string IP_addr, string mac_addr, string hostname) {
  Socket mng_socket;
  int send_res, recv_res;

  // Get a vector with all IPs to replicate the new participant to every active participant.
  auto IP_vec = MachinesManager::Instance().getVectorOfIPs();

  for (auto it = IP_vec.begin(); it != IP_vec.end(); ++it) {
    // Don't need to replicate the new participant to himself,
    // as this is already done in initialReplicationFor
    if (*it != IP_addr) {
      mng_socket.setSendAddr(*it, REPLICATION_PORT);

      // New Participant packet
      string msg = "[NP]" + IP_addr + '|' + mac_addr + '|' + hostname;
      send_res = mng_socket.sendMessage(msg);
      if (send_res < 0)
        cerr << ("[R] ERROR sendto") << endl;

      recv_res = mng_socket.receiveMessage();
    }
  }
}

// Replicates a new status, indicated by `status_code`, of the participant `IP_addr`
// to all participants. Internal method used by the public methods (replicate{status_name}Status).
// Message prefix: [NP]
void replicateNewStatus(string IP_addr, string status_code) {
  Socket mng_socket;
  int send_res, recv_res;

  // Get a vector with all IPs to replicate the new status to every active participant.
  auto IP_vec = MachinesManager::Instance().getVectorOfIPs();

  for (auto it = IP_vec.begin(); it != IP_vec.end(); ++it) {
    mng_socket.setSendAddr(*it, REPLICATION_PORT);

    // New Status packet
    string msg = status_code + IP_addr;
    send_res = mng_socket.sendMessage(msg);
    if (send_res < 0)
      cerr << ("[R] ERROR sendto") << endl;

    recv_res = mng_socket.receiveMessage();
  }
}

// Public method to replicate the AWAKE status of participant `IP_addr`.
void replicateAwakeStatus(string IP_addr) {
  replicateNewStatus(IP_addr, "[AW]");
}

// Public method to replicate the ASLEEP status of participant `IP_addr`.
void replicateAsleepStatus(string IP_addr) {
  replicateNewStatus(IP_addr, "[AS]");
}

// Public method to replicate the Participating status of participant `IP_addr`.
void replicateParticipatingStatus(string IP_addr) {
  replicateNewStatus(IP_addr, "[PA]");
}

// Public method to replicate the exit of participant `IP_addr` from monitoring.
void replicateExitStatus(string IP_addr) {
  replicateNewStatus(IP_addr, "[EX]");
}

// Parses the `message` to get data for a new participant, and creates it with default values for
// status and participating.
// Expected format:
// [NP]IP_addr|mac_addr|hostname
void newParticipantHandler(string message) {
  int divider1 = message.find('|');
  int divider2 = message.find('|', divider1 + 1);
  int data_start = message.find_last_of(']') + 1;
  string IP_addr = message.substr(data_start, divider1 - data_start);
  string mac_addr = message.substr(divider1 + 1, divider2 - divider1 - 1);
  string hostname = message.substr(divider2 + 1);

  // cout << "NP: " << IP_addr + ' ' + mac_addr + ' ' + hostname << endl;
  MachinesManager::Instance().createMachine(IP_addr, mac_addr, hostname);
}

// Parses the `message` to get data for a new participant, and creates it.
// Although similar to newParticipantHandler, it's used for initial replication, so needs more info.
// Expected format:
// [IR][X/Y]IP_addr|mac_addr|hostname|status|participating
void initialReplicationHandler(string message) {
  int divider1 = message.find('|');
  int divider2 = message.find('|', divider1 + 1);
  int divider3 = message.find('|', divider2 + 1);
  int divider4 = message.find('|', divider3 + 1);
  int divider5 = message.find('|', divider4 + 1);

  int data_start = message.find_last_of(']') + 1;
  int mach_count_start = message.find(']') + 2;
  string IP_addr = message.substr(data_start, divider1 - data_start);
  string mac_addr = message.substr(divider1 + 1, divider2 - divider1 - 1);
  string hostname = message.substr(divider2 + 1, divider3 - divider2 - 1);
  string status_s = message.substr(divider3 + 1, divider4 - divider3 - 1);
  string participating_s = message.substr(divider4 + 1, divider5 - divider4 - 1);
  string id_s = message.substr(divider5 + 1);
  int status = stoi(status_s);
  bool participating = (participating_s == "1");
  int id = stoi(id_s);
  int mach_count = stoi(message.substr(mach_count_start, 1));

  if (mach_count == 1) {
    MachinesManager::Instance().setNextId(0);
    // cout << "IR INITIATED!" << endl;
  }

  // cout << "IR: " << IP_addr + ' ' + mac_addr + ' ' + hostname + ' ' + status_s + ' ' + participating_s + ' ' + id_s << endl;
  MachinesManager::Instance().createMachine(id, IP_addr, mac_addr, hostname, status, participating);
}

// Parses the `message` to get data for a new participant's status, and updates it.
// Expected format:
// [status_code]IP_addr
void newStatusHandler(string message) {
  int IP_start = message.find_last_of(']') + 1;
  string new_status = message.substr(0, IP_start);
  string IP_addr = message.substr(IP_start);
  auto machine = MachinesManager::Instance().getMachine(IP_addr);

  // cout << "NS: " << new_status + ' ' + IP_addr << endl;
  if (new_status == "[AW]") {
    machine->second.setAwake();
  }
  else if (new_status == "[AS]") {
    machine->second.setAsleep();
  }
  else if (new_status == "[PA]") {
    machine->second.setParticipating(true);
  }
  else if (new_status == "[EX]") {
    MachinesManager::Instance().removeMachine(IP_addr);
  }
}

// Thread to listen for replication packets.
// Acts as a controller that routes each packet to the correct handler.
// Examples of received packets:
// [NP]192.168.15.135|3c:7c:3f:7b:8a:86|wpramio-pc
// [NP]192.168.15.98|98:83:89:c6:d7:83|wpramio-odyssey
// [AW]192.168.15.98
// [AS]192.168.15.98
// [EX]192.168.15.98
// [PA]192.168.15.98
// [IR][1/1]192.168.15.135|3c:7c:3f:7b:8a:86|wpramio-pc|1|1
void listenForReplicationPackets() {
  Socket ptcp_socket;
  int send_res, recv_res;

  ptcp_socket.setTimeoutOpt();
  ptcp_socket.listenPort(REPLICATION_PORT);

  while(!manager) {
    // Listen for packets sent by the manager to REPLICATION_PORT
    recv_res = ptcp_socket.receiveMessage(true);
    if (recv_res < 0){
      //cerr << "[R] ERROR recvfrom" << endl;
      continue;
    }

    string buffer = ptcp_socket.getBuffer();
    // log
    cerr << "[R] Rep msg: " << buffer << endl;

    // new participant
    if (buffer.substr(0, 4) == "[NP]") {
      newParticipantHandler(buffer);
    }
    // initial replication
    else if (buffer.substr(0, 4) == "[IR]") {
      initialReplicationHandler(buffer);
    }
    // new status
    else {
      newStatusHandler(buffer);
    }
    MachinesManager::Instance().setMapChanged(true);

    // Answers the packet received
    send_res = ptcp_socket.sendMessageToSender("Received");
    if (send_res < 0)
      // log
      cerr << "[R] ERROR sendto" << endl;
  }
  ptcp_socket.closeSocket();
  // log
  cerr << "[R] Exiting listenForReplicationPackets thread" << endl;
}
