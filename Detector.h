#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include "ArucoDetector.h"
#include "BodyPoseDetector.h"

using namespace cv;
using namespace std;

class Detector
{
public:
	void detectFeaturePositions(Mat image, vector<ArucoMarkerPosition>& aruco, vector<BodyPartPosition>& body) {
		arucoDetector.detect(image, aruco);
		bodyDetector.detect(image, body);
	}

private:
	ArucoDetector arucoDetector = ArucoDetector();
	BodyPoseDetector bodyDetector = BodyPoseDetector();

};

