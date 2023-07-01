#pragma once

#include <iostream>
#include <vector>

using namespace std;

struct TrackerNodeConfig {

	string serverHost;
	int serverPort;
	int numCameras;
	int frameWidth;
	int frameHeight;
	vector<string> cameraNames;

};
