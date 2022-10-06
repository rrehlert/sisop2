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
#define MAGIC_PORT 4005
#define BROADCAST_IP "255.255.255.255"

unsigned getHexFromString(const std::string& s)
{
    unsigned hex{0};
    
    for (size_t i = 0; i < s.length(); ++i) {
        hex <<= 4;
        if (isdigit(s[i])) {
            hex |= s[i] - '0';
        }
        else if (s[i] >= 'a' && s[i] <= 'f') {
            hex |= s[i] - 'a' + 10;
        }
        else if (s[i] >= 'A' && s[i] <= 'F') {
            hex |= s[i] - 'A' + 10;
        }
        else {
            throw std::runtime_error("Failed to parse hexadecimal " + s);
        }
    }
    return hex;
}



string convertMac(const string& hardware_addr){
    std::string ether_addr;

    for (size_t i = 0; i < hardware_addr.length();) {
        // Parse two characters at a time.
        unsigned hex = getHexFromString(hardware_addr.substr(i, 2));
        i += 2;

        ether_addr += static_cast<char>(hex & 0xFF);

        // We might get a colon here, but it is not required.
        if (hardware_addr[i] == ':') 
            ++i;
    }

    if (ether_addr.length() != 6)
        throw std::runtime_error(hardware_addr + " not a valid ether address");

    return ether_addr;
}


void sendMagicPacket(string mac){
    Socket magic_socket;
    int send_res, recv_res;

    // Convert string address to bytes
    const string mac_addr {convertMac(mac)};

    magic_socket.setBroadcastOpt();
	magic_socket.setSendAddr(BROADCAST_IP, MAGIC_PORT);

    // Build the message to send.
    //   (6 * 0XFF followed by 16 * destination address.)
    string message(6, 0xFF);
        for (size_t i = 0; i < 16; ++i) {
            message += mac_addr;
        }

    message = mac + message;

    // Send the packet out.
    cerr << "Sending magic packet to " << mac << endl;
    cerr << "Packet: " << message << endl;
    send_res = magic_socket.sendMessage(message);
    if (send_res < 0)
        cerr << "Error waking up" << endl;
}

void wakeupParticipant(string IP){ //Could change to hostname
    auto it = MachinesManager::Instance().getMachine(IP);
    if (it->second.isParticipating()){
        string mac_addr = it->second.getMac();
        cout << "Waking up machine: " << IP << endl;
        sendMagicPacket(mac_addr);
    }
    else
        cout << "Machine " << IP << " not participating" << endl;
}

