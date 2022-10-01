#pragma once
#include <string>
#include <iostream>
#include <time.h>
#include <map>
#include <vector>
#include <mutex>

#include "machine.cpp"

using namespace std;

static mutex map_mutex;

class MachinesManager {

  private:
    map<string, Machine> machines;
    bool map_changed = false;
    int next_id = 0;
    // Constructor and destructor must be private to ensure only one instance of the class
    // is created.
    MachinesManager() {}
    ~MachinesManager() {}

  public:
    static MachinesManager& Instance() {
      // Since it's a static variable, if the class has already been created,
      // it won't be created again.
      // And it **is** thread-safe in C++11.
      static MachinesManager myInstance;

      // Return a reference to our instance.
      return myInstance;
    }

    void incNextId() {
      next_id++;
    }

    void setNextId(int new_val) {
      next_id = new_val;
    }

    void setMapChanged(bool new_status) {
      map_mutex.lock();
      map_changed = new_status;
      map_mutex.unlock();
    }

    bool mapChanged() {
      map_mutex.lock();
      bool res = map_changed;
      map_mutex.unlock();

      return res;
    }

    bool machineIsKnown(string IP) {
      // Enters the critical section
      map_mutex.lock();

      bool res = (machines.count(IP) > 0);

      // Leaves the critical section
      map_mutex.unlock();

      return res;
    }

    void createMachine(string IP, string mac, string hostname) {
      incNextId();
      Machine mach(next_id, IP, mac, hostname);
      map_mutex.lock();

      machines[IP] = mach;
      map_mutex.unlock();
    }

    void createMachine(int ID, string IP, string mac, string hostname, int status, bool participating) {
      incNextId();
      Machine mach(ID, IP, mac, hostname, status, participating);
      map_mutex.lock();

      machines[IP] = mach;
      map_mutex.unlock();
    }

    auto getMachineByHostname(string hostname) {
      map_mutex.lock();

      auto res = machines.find(hostname);

      map_mutex.unlock();

      return res;
    }

    auto getMachine(string IP) {
      map_mutex.lock();

      auto res = machines.find(IP);

      map_mutex.unlock();

      return res;
    }

    auto getVectorOfIPs() {
      vector<string> vec;

      map_mutex.lock();

      for (auto it = machines.begin(); it != machines.end(); ++it)
        vec.push_back(it->second.getIP());

      map_mutex.unlock();

      return vec;
    }

    auto getVectorOfMachines() {
      vector<Machine> vec;

      map_mutex.lock();

      for (auto it = machines.begin(); it != machines.end(); ++it)
        vec.push_back(it->second);

      map_mutex.unlock();

      return vec;
    }

    void removeMachine(string IP){
      map_mutex.lock();
      auto it = machines.find(IP);
      it->second.setParticipating(false);
      it->second.restoreCount();

      map_mutex.unlock();
    }

    void printMachines(bool only_participating = false) {
      map_mutex.lock();
			for (auto it = machines.begin(); it != machines.end(); ++it) {
        if (only_participating == false || (only_participating == true && it->second.isParticipating() == true))
          it->second.print();
			}
      map_mutex.unlock();
    }
};
