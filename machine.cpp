#pragma once
#include <iostream>
#include <string>

#define AWAKE 1
#define ASLEEP 0
#define MAX_MISSED_CALLS 3

using namespace std;

class Machine {

	private:
		string hostname;
  	string IP_addr;
  	int status;
		unsigned int missed_calls;

  public:
		Machine(){};

		Machine(string IP, string _hostname) {
			hostname = _hostname;
			IP_addr = IP;
			status = AWAKE;
			missed_calls = 0;
		}

		Machine(const Machine& m1) {
			hostname = m1.hostname;
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

		void setAsleep() {
			status = ASLEEP;
		}

		string getIP() {
			return IP_addr;
		}

		void setIP(const string new_IP) {
			IP_addr = new_IP;
		}

		string getHostname() {
			return hostname;
		}

		void setHostname(const string new_hostname) {
			hostname = new_hostname;
		}

		unsigned int getMissedCalls() {
			return missed_calls;
		}

		void increaseMissedCalls() {
			missed_calls += 1;
			// TODO: move this check to Monitoring Subservice
			if (missed_calls >= MAX_MISSED_CALLS)
				setAsleep();
		}

		void resetMissedCalls() {
			missed_calls = 0;
		}

		bool isAwaken() {
			return (status == AWAKE);
		}

		bool isAsleep() {
			return (status == ASLEEP);
		}

		void print() {
			cout << "::Machine::" << '\n';
			cout << "Hostname: " << hostname << '\n';
			cout << "IP: " << IP_addr << '\n';
			cout << "Status: " << (status == AWAKE ? "awake" : "asleep") << '\n';
			cout << "Missed calls: " << missed_calls << endl;
		}
};
