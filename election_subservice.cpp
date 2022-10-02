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
bool answered = false;
map <int, Timeout> timeoutMap;
extern bool manager;
extern bool keep_monitoring;


void sendStartElectionPacket(string ip) {
  Socket socket;
  socket.setSendAddr(ip, ELECTION_PORT);

  //Send Start Election to the specified port
  cerr << "[E] Sending [SE] to " << ip << endl;
  socket.sendMessage("[SE]");
}

void sendAnswerElectionPacket(string ip) {
  Socket socket;
  socket.setSendAddr(ip, ELECTION_PORT);

  //Send Answer Election to the specified port
  cerr << "[E] Sending [AE] to " << ip << endl;
  socket.sendMessage("[AE]");
}

void sendElectedManagerPackets(string elected_ip) {
  Socket socket;

  auto vec = MachinesManager::Instance().getVectorOfMachines();

  for (Machine mac : vec) {
    if (mac.getIP() != elected_ip) {
      cerr << "[E] Sending [EM]" << elected_ip << " to " << mac.getIP() << endl;
      socket.setSendAddr(mac.getIP(), ELECTION_PORT);
      socket.sendMessage("[EM]" + elected_ip);
    }
  }
}

void startManagerElection() {
  if (isOnElection == false) {
    isOnElection = true;
    string myIp = getSelfIP();
    int myId = MachinesManager::Instance().getMachineId(myIp);

    // log
    cerr << "[E] Starting manager election" << endl;

    auto vec = MachinesManager::Instance().getVectorOfMachines();
    for (Machine mac : vec){
      if (mac.getID() > myId)
        sendStartElectionPacket(mac.getIP());
    }

    answered = false;
    timeoutMap[myId] = Timeout(myId, 5);

    int count = 0;
    while(!answered && count < timeoutMap[myId].duration) {
      sleep(1);
      count++;
      cerr << "[E] Timeout T, count: " << count << endl;
    }

    // if timeout completed
    if ( count == timeoutMap[myId].duration) {
      cerr << "[E] timeout T completed. Electing myself." << endl;
      manager = true;
      isOnElection = false;
      sendElectedManagerPackets(myIp);
      MachinesManager::Instance().setNewManager(myIp);
    }
    // if not completed, the next steps will be executed by answerElectionHandler()
  }
  // already on election
  else {
    cerr << "[E] Already on election, will not start a new one." << endl;
  }
}

void answerElectionHandler() {
  string myIp = getSelfIP();
  int myId = MachinesManager::Instance().getMachineId(myIp);

  answered = true;
  // timeoutMap[myId].stop();

  // comecar o timeout T'
  // reinicia election e seta isOnElection de todos pra false
}

void electedManagerHandler(string buffer) {
  string myIp = getSelfIP();
  int myId = MachinesManager::Instance().getMachineId(myIp);

  string ip = buffer.substr(4);
  int id = MachinesManager::Instance().getMachineId(ip);

  isOnElection = false;
  if (id > myId) {
    cerr << "[E] New manager accepted: " << id << endl;
    MachinesManager::Instance().setNewManager(ip);
    keep_monitoring = true;
  }
  else {
    cerr << "[E] New manager rejected: " << id << endl;
    manager = true;
    MachinesManager::Instance().setNewManager(myIp);
    sendElectedManagerPackets(myIp);
  }

  MachinesManager::Instance().setMapChanged(true);

  // Se id recebido é maior que o proprio
  //   atualiza a tabela
  // Se id recebido é menor que o proprio
  //   Envia o proprio id como manager packet pra todos
  //   atualiza a tabela
}

void startElectionHandler() {
  startManagerElection();
}

void listenForElectionPackets() {
  Socket socket;
  int send_res, recv_res;

  socket.listenPort(ELECTION_PORT);

  while(!manager) {
    // Listen for election packets
    recv_res = socket.receiveMessage();
    if (recv_res < 0){
      //cerr << "[R] ERROR recvfrom" << endl;
      // continue;
    }
    if (recv_res > 0) {
      string buffer = socket.getBuffer();
      // log
      cerr << "[E] Packet received: " << buffer << endl;

      // start election packet ([SE])
      if (buffer.substr(0, 4) == "[SE]") {
        sendAnswerElectionPacket(socket.getSenderIP());
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
    }
  }
  socket.closeSocket();
  cerr << "Exiting listenForElectionPackets thread" << endl;
}
