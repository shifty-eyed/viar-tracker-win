#pragma once

#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

#define THRESHOLD 0.1
#define NUM_PARTS 18
#define NUM_PAIRS 17

#define NET_BIN_FILE "e:/downloads/dev/openpos-models/graph_opt_thin.pb"
#define NET_TXT_FILE "e:/downloads/dev/openpos-models/graph_opt_thin.pbtxt"
#define USE_GPU true


struct BodyPartPosition {
	int id;
	vector<Point> points;
};

class BodyPoseDetector
{
private: 
	dnn::Net net;

public:
	BodyPoseDetector(String modelBin, String modelTxt, bool useGPU);

	BodyPoseDetector() : BodyPoseDetector(NET_BIN_FILE, NET_TXT_FILE, USE_GPU) {};

	virtual string detect(Mat image, vector<BodyPartPosition>& result);
};

