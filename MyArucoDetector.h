#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/aruco.hpp"

using namespace cv;
using namespace std;

struct ArucoMarkerPosition {
	int id;
	float x;
	float y;
	Point2f corners[4];
};

class MyArucoDetector 
{
public:
    MyArucoDetector();

    virtual void detect(Mat image, vector<ArucoMarkerPosition>& result);
private:
    Ptr<aruco::Dictionary> dictionary;
    Ptr<aruco::DetectorParameters> parameters;
};

