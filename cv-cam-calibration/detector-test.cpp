
#include <iostream>
#include <fstream> 
#include <filesystem> 
#include <math.h>
#include <stdio.h>
#include <time.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"

#include "../MyArucoDetector.h"
#include "../BodyPoseDetector.h"

using namespace std;
using namespace cv;

#define WINDOW_NAME "Testing Detector"

#define YELLOW Scalar(0,255,200)
#define BLUE Scalar(255,0,0)
#define GREEN Scalar(0,255,0)
#define RED Scalar(0,0,255)

#define EXPECTED_DETECTION_HITS 1
int actualDetectedHits = 0;
MyArucoDetector arucoDetector = MyArucoDetector();
//BodyPoseDetector bodyDetector = BodyPoseDetector();

const int POSE_PAIRS[NUM_PAIRS][2] = {   // COCO body
    {1,2}, {1,5}, {2,3},
    {3,4}, {5,6}, {6,7},
    {1,8}, {8,9}, {9,10},
    {1,11}, {11,12}, {12,13},
    {1,0}, {0,14},
    {14,16}, {0,15}, {15,17}
};

int doMagic(Mat &src, Mat& dst) {

    vector<ArucoMarkerPosition> arucos;
    vector<BodyPose> bodies;

    arucoDetector.detect(src, arucos);
    //string metrics = bodyDetector.detect(src, bodies);

    src.copyTo(dst);



    for (ArucoMarkerPosition m : arucos) {
        circle(dst, Point2f(m.x,m.y), 4, YELLOW, 2);
        for (int i = 0;i < 4;i++) {
            int next = i == 3 ? 0 : i + 1;
            line(dst, m.corners[i], m.corners[next], BLUE);
        }
    }


    //dro
    for (BodyPose body : bodies) {
        for (int n = 0; n < body.points.size(); n++ ) {
            Point2f p = body.points[n];
            if (p.x > 0 && p.y > 0) {
                circle(dst, p, 3, RED, 1);
                putText(dst, to_string(n), p, FONT_HERSHEY_SIMPLEX, 0.8f, BLUE, 2);
            }
        }
    }

    //putText(dst, metrics, Point(30, 30), FONT_HERSHEY_SIMPLEX, 0.5f, YELLOW, 1);

    return arucos.size();
}


int main(int argc, char* argv[]) {

    int camId = 0;//atoi(argv[1]);
    cout << "opening cam " << camId << std::endl;

    VideoCapture camera(camId, CAP_DSHOW);
    
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
        frameCount++;
        actualDetectedHits += doMagic(src, dst);

        cv::imshow(WINDOW_NAME, dst);


        if (clock() - t > CLOCKS_PER_SEC) {
            t = clock();
            fps = frameCount;
            double hitRate = (double)actualDetectedHits / (double)(EXPECTED_DETECTION_HITS * frameCount);
            printf("fps: %d, hitRate=%d%%\n", fps, (int)(hitRate*100.0));
            frameCount = 0;
            actualDetectedHits = 0;
        }
        
        char key = (char)cv::waitKey(1);
        if (key != -1) {
            break;
        }
    }
    return 0;

}

