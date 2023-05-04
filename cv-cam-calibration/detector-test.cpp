
#include <iostream>
#include <fstream> 
#include <filesystem> 
#include <math.h>
#include <stdio.h>
#include <time.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"

#include "../Detector.h"

using namespace std;
using namespace cv;

#define WINDOW_NAME "Testing Detector"

#define BLUE Scalar(0,255,200)

Detector detector = Detector();

void doMagic(Mat &src, Mat& dst) {

    vector<ArucoMarkerPosition> arucos;
    vector<BodyPartPosition> bodyParts;

    detector.detectFeaturePositions(src, arucos, bodyParts);
    src.copyTo(dst);


    for (ArucoMarkerPosition m : arucos) {
        circle(dst, Point2f(m.x,m.y), 4, BLUE, 2);
    }
}




int main(int argc, char* argv[]) {
    int camId = 0;//atoi(argv[1]);
    cout << "opening cam " << camId << std::endl;

    VideoCapture camera(camId);
    //camera.set(CAP_PROP_FRAME_WIDTH, 1920); camera.set(CAP_PROP_FRAME_HEIGHT, 1080);
    camera.set(CAP_PROP_FRAME_WIDTH, 1280); camera.set(CAP_PROP_FRAME_HEIGHT, 720);

    if (!camera.isOpened()) {
        cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }

    namedWindow(WINDOW_NAME, WINDOW_AUTOSIZE);

    Mat src;
    Mat dst;

    clock_t t = clock();
    int frameCount = 0;
    int fps = 0;
    while (1) {
        camera >> src;

        doMagic(src, dst);

        cv::imshow(WINDOW_NAME, dst);

        if (clock() - t > CLOCKS_PER_SEC) {
            t = clock();
            fps = frameCount;
            frameCount = 0;
            printf("fps: %d\n", fps);
        }
        frameCount++;
        char key = (char)cv::waitKey(1);
        if (key != -1) {
            break;
        }
    }
    return 0;

}

