
#include <iostream>
#include <fstream> 
#include <filesystem> 
#include <math.h>
#include <stdio.h>

#include "Utils.h"
#include "SocketConnectionWrapper.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"

#include "MyArucoDetector.h"
#include "BodyPoseDetector.h"
#include "TrackerNodeConfig.h"

using namespace std;
using namespace cv;

TrackerNodeConfig config;
SocketConnectionWrapper connection;

void parseConfig() {
	config.serverHost = "127.0.0.1";
	config.serverPort = 2023;
	config.numCameras = 1;
	config.cameraNames.push_back("cam1");
	config.cameraNames.push_back("cam2");
	config.cameraNames.push_back("cam3");
}

int main(int argc, char* argv[]) {
	parseConfig();
    if (!connection.init(config)) {
        cerr << connection.getLastError();
        return 1;
    }

    clock_t t = clock();
    clock_t tg = clock();
    int frameCount = 0;
    int fps = 0;
    while (1) {
        frameCount++;

        string msg = connection.receiveMessage();
        if (msg.empty() || msg == "end") {
            cerr << "Received terminated message: " << msg;
            break;
        };

        connection.sendMessage(fmt("Last fps is %d", fps));

        if (clock() - t > CLOCKS_PER_SEC) {
            t = clock();
            fps = frameCount;
            printf("fps: %d\n", fps);
            frameCount = 0;
        }

        if (clock() - tg > CLOCKS_PER_SEC * 5) {
            break;
        }

    }
    connection.close();
    return 0;
}