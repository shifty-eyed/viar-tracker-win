#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include "MyArucoDetector.h"
#include "BodyPoseDetector.h"

using namespace cv;
using namespace std;



class Detector
{
public:
	string detectFeaturePositions(Mat image, vector<ArucoMarkerPosition>& aruco, vector<BodyPartPosition>& body) {
		arucoDetector.detect(image, aruco);
		return bodyDetector.detect(image, body);
	}

	Detector() {
		arucoDetector = MyArucoDetector();
		bodyDetector = BodyPoseDetector(NET_BIN_FILE, NET_TXT_FILE, false);
	}

private:
	MyArucoDetector arucoDetector = MyArucoDetector();
	BodyPoseDetector bodyDetector = BodyPoseDetector(NET_BIN_FILE, NET_TXT_FILE, false);

};

