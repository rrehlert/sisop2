#pragma once
#include <iostream>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "participant_subservice.cpp"
#include "wakeup_subservice.cpp"
#include "management_subservice.cpp"

#define POOL_INTERVAL 1

using namespace std;

extern bool manager;

void signal_handler(int signum){
    cout << " Handling Ctrl + C " << endl;
    sendExitPacket();
    // Terminate program
    exit(0);
}

void printHeader(bool for_manager = true) {
    cout.width(25); cout << left << "Hostname";
    cout.width(15); cout << left << "IP";
    cout.width(21); cout << left << "Mac";
    if (for_manager == true) {
        cout.width(11); cout << left << "Status";
        cout.width(13); cout << left << "Participating" << endl;
    }
    else
        cout << endl;
}

void printManager() {
    cout.width(25); cout << left << manager_hostname;
    cout.width(15); cout << left << manager_ip;
    cout.width(21); cout << left << manager_mac << endl;
}

void read_CLI(){
    string command;
    signal(SIGINT, signal_handler);
    while (true){
        cin.clear();
        cin >> command;
        
        if (cin.eof()){
            cout << " Handling Ctrl + D" << endl;
            sendExitPacket();
            exit(0);
        }

        if ((command.compare("EXIT") == 0 && manager == false)){
            cout << " Handling EXIT" << endl;
            sendExitPacket();
            exit(0);
        }
        if ((command.compare("WAKEUP") == 0 && manager == true)){
            cin >> command;
            wakeupParticipant(command);
        }
    }
}

void updateManagerInterface() {
    while(true) {
        if (MachinesManager::Instance().mapChanged() == true) {
            system("clear");
            cout << "Role: [M]" << endl;
            cout << "Participants:" << endl;
            printHeader();
            MachinesManager::Instance().printMachines();
            MachinesManager::Instance().setMapChanged(false);
        }

        sleep(POOL_INTERVAL);
    }
}

void updateParticipantInterface() {
    while(true) {
        if (manager_changed == true) {
            system("clear");
            cout << "Role: [P]" << endl;
            cout << "Latest manager info:" << endl;
            printHeader(false);
            printManager();
            manager_changed = false;
        }

        sleep(POOL_INTERVAL);
    }
}
