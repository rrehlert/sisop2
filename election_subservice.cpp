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


void sendStartElectionPacket(string ip) {
  Socket socket;
  socket.setSendAddr(ip, ELECTION_PORT);

  //Send start election to the specified port
  socket.sendMessage("[SE]");
}

void sendElectedManagerPackets(string elected_ip) {
  Socket socket;
  
  auto vec = MachinesManager::Instance().getVectorOfMachines();

  for (Machine mac : vec){
    socket.setSendAddr(mac.getIP(), ELECTION_PORT);
    socket.sendMessage("[EM]" + elected_ip);
  }
}

void startManagerElection() {
  string ip = getSelfIP();
  int myId = MachinesManager::Instance().getMachineId(ip);

  // log
  cerr << "[E] Starting manager election" << endl;

  auto vec = MachinesManager::Instance().getVectorOfMachines();
  for (Machine mac : vec){
    if (mac.getID() > myId)
      sendStartElectionPacket(mac.getIP());
  }
}

void answerElectionHandler() {
  string ip = getSelfIP();
  int myId = MachinesManager::Instance().getMachineId(ip);
  timeoutMap[myId].stop();

  // comecar o timeout T'
  // reinicia election e seta isOnElection de todos pra false
}

void electedManagerHandler(string buffer) {
  string myIp = getSelfIP();
  int myId = MachinesManager::Instance().getMachineId(myIp);

  string ip = buffer.substr(5);
  int id = MachinesManager::Instance().getMachineId(ip);

  if (id > myId) {
    MachinesManager::Instance().setNewManager(ip);
  }
  else {
    becomeManager();
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

void listenForElectionPackets() {
  Socket socket;
  int send_res, recv_res;

  socket.listenPort(ELECTION_PORT);

  while(true) {
    // Listen for election packets
    recv_res = socket.receiveMessage(true);
    if (recv_res < 0){
      //cerr << "[R] ERROR recvfrom" << endl;
      continue;
    }

    string buffer = socket.getBuffer();
    // log
    cerr << "Election msg: " << buffer << endl;

    // start election packet ([SE])
    if (buffer.substr(0, 4) == "[SE]") {
      send_res = socket.sendMessageToSender("[AE]");
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
  }
  socket.closeSocket();
  cerr << "Exiting listenForElectionPackets thread" << endl;
}
