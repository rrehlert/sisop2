#pragma once
#include <iostream>
#include <string>

#define AWAKE 1
#define ASLEEP 0

using namespace std;

class Machine {

	private:
		string hostname;
  	string IP_addr;
  	int status;
		bool participating;

  public:
		Machine(){};

		Machine(string IP, string _hostname) {
			hostname = _hostname;
			IP_addr = IP;
			status = AWAKE;
			participating = true;
		}

		Machine(const Machine& m1) {
			hostname = m1.hostname;
			IP_addr = m1.IP_addr;
			status = m1.status;
			participating = m1.participating;
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

		bool isAwake() {
			return (status == AWAKE);
		}

		bool isAsleep() {
			return (status == ASLEEP);
		}

		bool isParticipating() {
			return participating;
		}

		void setParticipating(bool new_status) {
			participating = new_status;
		}

		void print() {
			cout << "::Machine::" << '\n';
			cout << "Hostname: " << hostname << '\n';
			cout << "IP: " << IP_addr << '\n';
			cout << "Status: " << (status == AWAKE ? "awake" : "asleep") << '\n';
			cout << "Participating: " << (participating == true ? "true" : "false") << endl;
		}
};
