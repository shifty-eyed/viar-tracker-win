#include "MyArucoDetector.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include <opencv2/mcc.hpp>

using namespace std;
using namespace cv;
using namespace cv::aruco;

MyArucoDetector::MyArucoDetector()
{
	dictionary = makePtr<Dictionary>(cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100));
	//parameters = makePtr<DetectorParameters>;
}

void MyArucoDetector::detect(Mat image, vector<ArucoMarkerPosition>& result)
{
    vector<int> markerIds;
    vector<vector<Point2f>> markerCorners;

    detectMarkers(image, dictionary, markerCorners, markerIds);

    if (markerIds.empty()) {
        return;
    }

    for (int i = 0; i < markerIds.size(); i++) {
        ArucoMarkerPosition m;
        for (int c = 0; c < 4; c++) {
            m.corners[c] = markerCorners.at(i).at(c);
        }
        m.x = (m.corners[0].x + m.corners[2].x) / 2;
        m.y = (m.corners[0].y + m.corners[2].y) / 2;
        m.id = markerIds[i];
        result.push_back(m);
    }
}
