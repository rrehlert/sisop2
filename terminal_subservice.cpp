#pragma once
#include <iostream>
#include <string.h>
#include <signal.h>
#include "participant_subservice.cpp"
#include "wakeup_subservice.cpp"

using namespace std;

extern bool manager;

void signal_handler(int signum){
    cout << " Handling Ctrl + C " << endl;
    sendExitPacket();
    // Terminate program
    exit(0);
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
            sendExitPacket();
            exit(0);
        }
        if ((command.compare("WAKEUP") == 0 && manager == true)){
            cin >> command;
            wakeupParticipant(command);
        }
    }
}

