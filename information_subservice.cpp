#pragma once
#include <iostream>
#include <fstream>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <net/if.h>
#include <stdlib.h>

using namespace std;


string exec(string command) {
  char buffer[128];
  string result = "";

  // Open pipe to file
  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    return "popen failed!";
  }

  // read till end of process:
  while (!feof(pipe)) {

    // use buffer to read and add to result
    if (fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }

  pclose(pipe);
  return result;
}


string getSelfHostname(){
  ifstream file;
  string path = "/etc/hostname";
  string hostname = "";
  file.open(path);
  if(!file.fail())
    file >> hostname;
  return hostname;
}

string getMacAddress(){
  string command; 
  string mac;

  //Execute terminal command to get ethernet information
  command = exec("ip a | grep -A1 enp");
  if(command.length() == 0)
    command = exec("ip a | grep -A1 eth0");
  int pos = command.find("link/ether");

  //Scan for mac address
  mac = command.substr(pos+11, 17);
  return mac;
}

string getSelfIP(){
  // Execute terminal command to get IP information
  string command = exec(R"(ip -4 addr | grep -oP '(?<=inet\s)192+(\.\d+){3}')");
  
  // Trim trailing whitespaces 
  string whitespaces (" \t\f\v\n\r");
  size_t last_non_whitespace = command.find_last_not_of(whitespaces);
  if (last_non_whitespace != string::npos)
    command.erase(last_non_whitespace + 1);

  return command;
}
