#pragma once

#include <iostream>
#include <vector>

using namespace std;

struct TrackerNodeConfig {

	string serverHost;
	int serverPort;
	int numCameras;
	vector<string> cameraNames;

};
