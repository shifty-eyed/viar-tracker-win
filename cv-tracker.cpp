
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
#include "JsonSerializer.h"

using namespace std;
using namespace cv;

TrackerNodeConfig config;
SocketConnectionWrapper connection;
vector<VideoCapture> cameras;

MyArucoDetector arucoDetector = MyArucoDetector();
BodyPoseDetector bodyDetector = BodyPoseDetector();
JsonSerializer serializer = JsonSerializer();

void parseConfig() {
	config.serverHost = "127.0.0.1";
	config.serverPort = 2023;
    config.frameWidth = 1280;
    config.frameHeight = 720;
	config.numCameras = 1;
	config.cameraNames.push_back("cam1");
	config.cameraNames.push_back("cam2");
	config.cameraNames.push_back("cam3");
}

bool initCameras() {
    for (int i = 0; i < config.numCameras; i++) {
        VideoCapture camera(i, CAP_DSHOW);
        camera.set(CAP_PROP_FRAME_WIDTH, config.frameWidth);
        camera.set(CAP_PROP_FRAME_HEIGHT, config.frameHeight);
        if (!camera.isOpened()) {
            cerr << "ERROR: Could not open camera" << std::endl;
            return false;
        }
        cameras.push_back(camera);
    }
    return true;
}

void processCaptures() {
    Mat image;
    vector<ArucoMarkerPosition> arucos;
    vector<BodyPose> bodies;
    for (int i = 0; i < config.numCameras; i++) {
        string cameraName = config.cameraNames[i];
        VideoCapture camera = cameras[i];
        camera >> image;
        
        arucos.clear();
        bodies.clear();
        arucoDetector.detect(image, arucos);
        bodyDetector.detect(image, bodies);

        serializer.addCapture(cameraName, arucos, bodies);
    }
}

int main(int argc, char* argv[]) {
	parseConfig();
    if (!connection.init(config)) {
        cerr << connection.getLastError();
        return 1;
    }
    if (!initCameras()) {
        return 1;
    }

    clock_t t = clock();
    int frameCount = 0;
    int fps = 0;
    while (1) {

        string msg = connection.receiveMessage();
        if (msg == "end") {
            cerr << "Received terminated message: " << msg;
            break;
        };
        

        frameCount++;
        serializer.clear();
        processCaptures();

        connection.sendMessage("begin\n");
        connection.sendMessage(serializer.toString());
        connection.sendMessage("end\n");

        if (clock() - t > CLOCKS_PER_SEC) {
            t = clock();
            fps = frameCount;
            printf("fps: %d\n", fps);
            frameCount = 0;
        }

    }
    connection.close();
    return 0;
}