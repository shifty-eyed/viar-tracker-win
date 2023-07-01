#pragma once

#include <iostream>
#include <sstream>

#include "MyArucoDetector.h"
#include "BodyPoseDetector.h"

class JsonSerializer {

public: 
	void clear() { buffer.clear(); }
	string toString() { return buffer.str(); };

	void addCapture(string cameraName, vector<ArucoMarkerPosition> arucos, vector<BodyPose> bodies);

private:
	stringstream buffer;

	void addArucoMarkerPosition(ArucoMarkerPosition data);
	void addBodyPose(BodyPose data);
	void addPoint2f(Point2f p);

};

