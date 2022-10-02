#pragma once
#include <string>
#include <iostream>
#include <time.h>
#include <map>
#include <thread>

#include "socket.cpp"
#include "machine.cpp"
#include "timeout.cpp"
#include "management_subservice.cpp"
#include "monitoring_subservice.cpp"
#include "information_subservice.cpp"
#include "replication_subservice.cpp"
#include "participant_subservice.cpp"

#define ELECTION_PORT 4006

using namespace std;

bool isOnElection = false;
map <int, Timeout> timeoutMap;
void listenForElectionPackets() {
  Socket ptcp_socket;
  int send_res, recv_res;

  ptcp_socket.setTimeoutOpt();
  ptcp_socket.listenPort(ELECTION_PORT);

  while(true) {
    // Listen for election packets
    recv_res = ptcp_socket.receiveMessage(true);
    if (recv_res < 0){
      //cerr << "[R] ERROR recvfrom" << endl;
      continue;
    }

    string buffer = ptcp_socket.getBuffer();
    // cout << "Rep msg: " << buffer << endl;

    // start election packet ([SE])
    if (buffer.substr(0, 4) == "[SE]") {
      send_res = ptcp_socket.sendMessageToSender("[AE]");
      if (send_res < 0)
        cerr << "[R] ERROR sendto" << endl;
      startElectionHandler();
    }
    // answer election packet ([AE])
    else if (buffer.substr(0, 4) == "[AE]") {
      answerElectionHandler();
    }
    // elected manager packet ([EM])
    else {
      electedManagerHandler(buffer);
    }

    MachinesManager::Instance().setMapChanged(true);

    // Answers the packet received
    send_res = ptcp_socket.sendMessageToSender("Received");
    if (send_res < 0)
      cerr << "[R] ERROR sendto" << endl;
  }
  ptcp_socket.closeSocket();
  //cout << "Exiting Thread 3";
}

void answerElectionHandler() {
  string ip = getSelfIP();
  auto myMachine =  MachinesManager::Instance().getMachine(ip);
  int myId = myMachine->second.getID();
  timeoutMap[myId].stop();

  // comecar o timeout T'
  // reinicia election e seta isOnElection de todos pra false
}

void electedManagerHandler(string buffer) {
  string myIp = getSelfIP();
  auto myMachine =  MachinesManager::Instance().getMachine(myIp);
  int myId = myMachine->second.getID();

  string ip = buffer.substr(5);
  auto machine =  MachinesManager::Instance().getMachine(ip);
  int id = machine->second.getID();

  if (id > myId) {
    MachinesManager::Instance().setNewManager(ip);
  }

  // Se id recebido é maior que o proprio
  //   atualiza a tabela
  // Se id recebido é menor que o proprio
  //   Envia o proprio id como manager packet pra todos
  //   atualiza a tabela
}

void startElectionHandler() {
  if (isOnElection == false) {
    isOnElection = true;
    string ip = getSelfIP();
    auto myMachine =  MachinesManager::Instance().getMachine(ip);
    int myId = myMachine->second.getID();

    auto vec = MachinesManager::Instance().getVectorOfMachines();
    for (Machine mac : vec){
      if (mac.getID() > myId)
        sendStartElectionPacket(mac.getIP());
    }

   timeoutMap[myId] = Timeout(myId, 5);

    // If timeout ocurred
    int count = 0;

    while(timeoutMap[myId].running == true || count < timeoutMap[myId].duration) {
      sleep(1);
      count++;
   
    }

    // if not stopped
    if ( timeoutMap[myId].running == true) {
      becomeManager();
      sendElectedManagerPackets(ip);
    }
    // if stopped, this will be handled by answerElectionHandler().
  }
}

void sendStartElectionPacket(string ip) {
  Socket election_socket;
  election_socket.setSendAddr(ip, ELECTION_PORT);

  //Send start election to he specified port
  election_socket.sendMessage("[SE]");
}

void sendElectedManagerPackets(string elected_ip) {
  Socket election_socket;
  
  auto vec = MachinesManager::Instance().getVectorOfMachines();

  for (Machine mac : vec){
    election_socket.setSendAddr(mac.getIP(), ELECTION_PORT);
    election_socket.sendMessage("[EM]" + elected_ip);
  }
}