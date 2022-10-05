#pragma once
#include <iostream>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "participant_subservice.cpp"
#include "wakeup_subservice.cpp"
#include "management_subservice.cpp"
#include "manager_subservice.cpp"

#define POLL_INTERVAL 1

using namespace std;

extern bool manager;

void signal_handler(int signum){
    cout << " Handling Ctrl + C " << endl;
    sendExitPacket();
    // Terminate program
    exit(0);
}

void printHeader(bool for_manager = true) {
    if (for_manager) {
        cout.width(5); cout << left << "ID";
    }
    cout.width(25); cout << left << "Hostname";
    cout.width(17); cout << left << "IP";
    cout.width(20); cout << left << "MAC";
    if (for_manager) {
        cout.width(11); cout << left << "Status";
    }
    cout << endl;
}

void printManager() {
    cout.width(25); cout << left << manager_hostname;
    cout.width(17); cout << left << manager_ip;
    cout.width(20); cout << left << manager_mac << endl;
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
        else if ((command.compare("WAKEUP") == 0 && manager == true)){
            cin >> command;
            wakeupParticipant(command);
        }
        else if ((command.compare("MANAGER") == 0 && manager == false)){
            cout << " Turning into MANAGER" << endl;
            becomeManager();
        }
        else if ((command.compare("PARTICIPANT") == 0 && manager == true)){
            cout << " Turning into PARTICIPANT" << endl;
            becomeParticipant();
        }
        else {
            cout << "Invalid command!" << endl;
        }
    }
}

void updateManagerInterface() {
    while(manager) {
        if (MachinesManager::Instance().mapChanged() == true) {
            system("clear");
            cout << "Role: Manager" << endl;
            cout << "Participants:" << endl;
            printHeader();
            MachinesManager::Instance().printMachines();
            MachinesManager::Instance().setMapChanged(false);
        }

        sleep(POLL_INTERVAL);
    }
}

void updateParticipantInterface() {
    while(!manager) {
        if (manager_changed == true || MachinesManager::Instance().mapChanged() == true) {
            system("clear");
            cout << "Role: Participant" << endl;
            cout << "Latest manager info:" << endl;
            printHeader(false);
            printManager();
            manager_changed = false;

            // REMOVE THIS FOR PRODUCTION
            // Print machines table to supervise the replication
            cout << '\n' << "Participants:" << endl;
            printHeader();
            MachinesManager::Instance().printMachines();
            MachinesManager::Instance().setMapChanged(false);
        }

        sleep(POLL_INTERVAL);
    }
    //cout << "Exiting Thread 4";
}
