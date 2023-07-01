#include "JsonSerializer.h"

void JsonSerializer::addCapture(string cameraName, vector<ArucoMarkerPosition> arucos, vector<BodyPose> bodies) {
	buffer << "{";
	buffer << "\"cameraName\": \"" << cameraName << "\",";
	buffer << "\"arucos\": [";
	for (int i = 0; i < arucos.size(); i++) {
		addArucoMarkerPosition(arucos[i]);
		if (i < arucos.size() - 1) {
			buffer << ",";
		}
	}
	buffer << "],";

	buffer << "\"bodies\": [";
	for (int i = 0; i < bodies.size(); i++) {
		addBodyPose(bodies[i]);
		if (i < bodies.size() - 1) {
			buffer << ",";
		}
	}
	buffer << "]";
	buffer << "}\n";
}

void JsonSerializer::addArucoMarkerPosition(ArucoMarkerPosition data) {
	buffer << "{";
	buffer << "\"id\":\"" << data.id << "\",";
	buffer << "\"x\":\"" << data.x << "\",";
	buffer << "\"y\":\"" << data.y << "\",";

	buffer << "\"corners\":[";
	for (int i = 0; i < 4; i++) {
		addPoint2f(data.corners[i]);
		if (i < 3) {
			buffer << ",";
		}
	}
	buffer << "],";
	buffer << "}";
}

void JsonSerializer::addBodyPose(BodyPose data) {
	buffer << "{";
	buffer << "\"id\":\"" << data.id << "\",";

	buffer << "\"points\":[";
	for (int i = 0; i < data.points.size(); i++) {
		addPoint2f(data.points[i]);
		if (i < data.points.size() - 1) {
			buffer << ",";
		}
	}
	buffer << "]";
	buffer << "}";
}

void JsonSerializer::addPoint2f(Point2f p) {
	buffer << "{";
	buffer << "\"x\":\"" << p.x << "\",";
	buffer << "\"y\":\"" << p.y << "\"";
	buffer << "}";
}
