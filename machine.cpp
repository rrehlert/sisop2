#pragma once
#include <iostream>
#include <string>

#define AWAKEN 1
#define SLEEPY 0

using namespace std;

class Machine {

	private:
		string host_name;
  	string IP_addr;
  	int status;

  public:
		Machine(){};

		Machine(string IP, string hostname) {
			host_name = hostname;
			IP_addr = IP;
			status = AWAKEN;
		}

		Machine(const Machine& m1) {
			host_name = m1.host_name;
			IP_addr = m1.IP_addr;
			status = m1.status;
		}

		int getStatus() {
			return status;
		}

		void setStatus(const int new_status) {
			status = new_status;
		}

		void setAwaken() {
			status = AWAKEN;
		}

		void setSleepy() {
			status = SLEEPY;
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

		bool isAwaken() {
			return (status == AWAKEN);
		}

		void print() {
			cout << "::Machine::" << endl;
			cout << "IP: " << IP_addr << endl;
			cout << "Hostname: " << host_name << endl;
			cout << "Status: " << (status == AWAKEN ? "awaken" : "sleepy") << endl;
		}
};
