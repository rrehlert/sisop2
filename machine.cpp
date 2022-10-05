#pragma once
#include <iostream>
#include <string>

#define OFF 2
#define AWAKE 1
#define ASLEEP 0

using namespace std;

class Machine {

	private:
		int id;
		string hostname;
  	string IP_addr;
		string mac_addr;
  	int status;
		int participating_count;
		bool is_manager;

  public:
		Machine(){};

		Machine(int _id, string IP, string mac, string _hostname, int _status = AWAKE) {
			id = _id;
			hostname = _hostname;
			IP_addr = IP;
			mac_addr = mac;
			status = _status;
			participating_count = 2;
			is_manager = false;
		}

		Machine(const Machine& m1) {
			id = m1.id;
			hostname = m1.hostname;
			IP_addr = m1.IP_addr;
			mac_addr = m1.mac_addr;
			status = m1.status;
			participating_count = m1.participating_count;
			is_manager = m1.is_manager;
		}

		static bool compareMachineIDs(const Machine& m1, const Machine& m2) {
			return (m1.id < m2.id);
		}

		int getID() {
			return id;
		}

		int getCount() {
			return participating_count;
		}

		int getStatus() {
			return status;
		}

		void restoreCount(){
			participating_count = 0;
		}

		void incrementCount(){
			participating_count ++;
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

		void setOff() {
			status = OFF;
		}

		void setIsManager(bool new_value) {
			is_manager = new_value;
		}

		string getMac(){
			return mac_addr;
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
			return (status != OFF);
		}

		bool isManager() {
			return is_manager;
		}

		string getStatusString() {
			string res;
			switch (status) {
				case AWAKE:
					res = "awake";
					break;
				case ASLEEP:
					res = "asleep";
					break;
				case OFF:
					res = "off";
					break;
				default:
					res = "undefined";
					break;
			}

			return res;
		}

		void print() {
			cout.width(5);  cout << left << id;
			cout.width(25); cout << left << hostname;
			cout.width(17); cout << left << IP_addr;
			cout.width(20); cout << left << mac_addr;
			cout.width(11); cout << left << getStatusString();
			cout.width(8);  cout << left << (is_manager ? "MANAGER" : "") << endl;
		}

		void printLog() {
			cerr.width(3);  cerr << left << id;
			cerr.width(25); cerr << left << hostname;
			cerr.width(16); cerr << left << IP_addr;
			cerr.width(18); cerr << left << mac_addr;
			cerr.width(8);  cerr << left << getStatusString();
			cerr.width(8);  cerr << left << (is_manager ? "MANAGER" : "") << endl;
		}
};
