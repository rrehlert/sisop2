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

#define PORT 4000
#define MAGIC_PORT 9
#define BROADCAST_IP "255.255.255.255"

void sendMagicPacket(string mac_addr){
    Socket magic_socket;
    int send_res, recv_res;

    magic_socket.setBroadcastOpt();
	magic_socket.setSendAddr(BROADCAST_IP, MAGIC_PORT);
    // Build the message to send.
    //   (6 * 0XFF followed by 16 * destination address.) 
    string message(6, 0xFF);
        for (size_t i = 0; i < 16; ++i) {
            message += mac_addr;
        }

    // Send the packet out. 
    cout << message << endl;
    send_res = magic_socket.sendMessage(message);
    if (send_res < 0)
        cerr << "Error sending message" << endl;
}

void wakeupParticipant(string IP){ //Change to hostname
    auto it = MachinesManager::Instance().getMachine(IP);
    string mac_addr = it->second.getMac();
    sendMagicPacket(mac_addr);
}
