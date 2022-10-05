#pragma once
#include <string>
#include <iostream>
#include <time.h>
#include <map>
#include <vector>
#include <mutex>
#include <algorithm>

#include "machine.cpp"

using namespace std;

static mutex map_mutex;

class MachinesManager {

  private:
    map<string, Machine> machines;
    bool map_changed = false;
    int next_id = 1;
    string manager_ip = "";
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

    int getNextId() {
      return next_id;
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
      Machine mach(next_id, IP, mac, hostname);
      map_mutex.lock();

      machines[IP] = mach;
      map_mutex.unlock();
      incNextId();
    }

    void createMachine(int ID, string IP, string mac, string hostname, int status) {
      Machine mach(ID, IP, mac, hostname, status);
      map_mutex.lock();

      machines[IP] = mach;
      map_mutex.unlock();
      next_id = ID + 1;
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

    int getMachineId(string IP) {
      return getMachine(IP)->second.getID();
    }

    auto getVectorOfMachines() {
      vector<Machine> vec;

      map_mutex.lock();

      for (auto it = machines.begin(); it != machines.end(); ++it)
        vec.push_back(it->second);

      map_mutex.unlock();

      // sort by id
      sort(vec.begin(), vec.end(), Machine::compareMachineIDs);
      return vec;
    }

    void setNewManager(string ip) {
      map_mutex.lock();

      // unset the old manager, if there was one
      if (manager_ip != "") {
        machines.find(manager_ip)->second.setIsManager(false);
      }
      // set the new manager
      machines.find(ip)->second.setIsManager(true);
      manager_ip = ip;

      map_mutex.unlock();
    }

    void removeMachine(string IP){
      map_mutex.lock();
      auto it = machines.find(IP);
      it->second.setOff();
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
