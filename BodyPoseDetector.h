#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

struct BodyPartPosition {
	int id;
	float x;
	float y;
};

class BodyPoseDetector
{
public:
	BodyPoseDetector();

	virtual void detect(Mat image, vector<BodyPartPosition>& result);
};

