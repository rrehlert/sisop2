#pragma once
#include <iostream>
#include <string>

#define AWAKE 1
#define SLEEPING 0
#define MAX_MISSED_CALLS 3

using namespace std;

class Machine {

	private:
		string host_name;
  	string IP_addr;
  	int status;
		unsigned int missed_calls;

  public:
		Machine(){};

		Machine(string IP, string hostname) {
			host_name = hostname;
			IP_addr = IP;
			status = AWAKE;
			missed_calls = 0;
		}

		Machine(const Machine& m1) {
			host_name = m1.host_name;
			IP_addr = m1.IP_addr;
			status = m1.status;
			missed_calls = m1.missed_calls;
		}

		int getStatus() {
			return status;
		}

		void setStatus(const int new_status) {
			status = new_status;
		}

		void setAwake() {
			status = AWAKE;
		}

		void setSleeping() {
			status = SLEEPING;
		}

		string getIP() {
			return IP_addr;
		}

		void setIP(const string new_IP) {
			IP_addr = new_IP;
		}

		string getHostname() {
			return host_name;
		}

		void setHostname(const string new_host_name) {
			host_name = new_host_name;
		}

		unsigned int getMissedCalls() {
			return missed_calls;
		}

		void increaseMissedCalls() {
			missed_calls += 1;
			if (missed_calls >= MAX_MISSED_CALLS)
				status = SLEEPING;
		}

		void resetMissedCalls() {
			missed_calls = 0;
		}

		bool isAwaken() {
			return (status == AWAKE);
		}

		void print() {
			cout << "::Machine::" << '\n';
			cout << "IP: " << IP_addr << '\n';
			cout << "Hostname: " << host_name << '\n';
			cout << "Status: " << (status == AWAKE ? "awake" : "sleeping") << '\n';
			cout << "Missed calls: " << missed_calls << endl;
		}
};
