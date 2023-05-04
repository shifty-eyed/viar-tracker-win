
#include <iostream>
#include <fstream> 
#include <filesystem> 
#include <math.h>
#include <stdio.h>
#include <time.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/aruco.hpp"

using namespace std;
using namespace cv;



void generateArucoMarkers() {
    Mat markerImage;
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_4X4_100);
    char filename[50];
    for (int i = 0; i < 20; i++) {
        aruco::drawMarker(dictionary, i, 256, markerImage, 1);
        sprintf_s(filename, "d:/ws/cv-tracker/img/DICT_4X4_100-marker-%02d.png", i);
        imwrite(filename, markerImage);
    }
}

int main1(int argc, char* argv[]) {

    generateArucoMarkers();

}