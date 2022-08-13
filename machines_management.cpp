#pragma once
#include <string>
#include <iostream>
#include <time.h>
#include <map>

#include "machine.cpp"

using namespace std;

class MachinesManager {

  private:
    map<string, Machine> machines;

    MachinesManager() {
    // Constructor code goes here.
    }

    ~MachinesManager() {
      // Destructor code goes here.
    }

  public:
    static MachinesManager& Instance() {
    // Since it's a static variable, if the class has already been created,
    // it won't be created again.
    // And it **is** thread-safe in C++11.
    static MachinesManager myInstance;

    // Return a reference to our instance.
    return myInstance;
    }

    bool machineIsKnown(string IP) {
      return (machines.count(IP) > 0);
    }

    void createMachine(string hostname, string IP) {
      Machine mach(IP, hostname);
      machines[IP] = mach;
    }

    auto getMachine(string IP) {
      return machines.find(IP);
    }

    void increaseMissedCalls() {
      for (auto it = machines.begin(); it != machines.end(); ++it)
				it->second.increaseMissedCalls();
    }

    void printMachines() {
      cout << "** Machines Map **" << endl;
			for (auto it = machines.begin(); it != machines.end(); ++it) {
				cout << it->first << " => " << endl;
				it->second.print();
				cout << '\n';
			}
    }
};
