#pragma once
#include <iostream>
#include <string>
#include <time.h>

#define AWAKE 1
#define ASLEEP 0

using namespace std;

class Timeout {

  public:
		int owner_id;
		int duration;
  	bool running;

		Timeout(){};

		Timeout(int _owner_id, int _duration, bool _running = false) {
      owner_id = _owner_id;
      duration = _duration;
      running = _running;
		}

    void stop() {
      running = false;
    }
};