#include "org_viar_core_CameraProcessor.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"

using namespace std;
using namespace cv;
using namespace cv::aruco;

vector<VideoCapture*> cameras;

cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_100);


JNIEXPORT void JNICALL Java_org_viar_core_CameraProcessor_init (JNIEnv* env, jobject instance, jint cameraCount) {

    for (int cameraId = 0; cameraId < cameraCount; cameraId++) {
        VideoCapture* camera = new VideoCapture(cameraId);
        camera->set(CAP_PROP_FRAME_WIDTH, 1920);
        camera->set(CAP_PROP_FRAME_HEIGHT, 1080);
        cameras.push_back(camera);
        if (camera->isOpened()) {
            std::cout << "Using camera slot " << cameraId << std::endl;
        } else {
            std::cerr << "Failed camera slot " << cameraId << std::endl;
            camera->release();
        }
    }
}

JNIEXPORT jobjectArray JNICALL Java_org_viar_core_CameraProcessor_processFrame (JNIEnv* env, jobject instance, jint cameraId) {
    jobjectArray result;
    VideoCapture* camera = cameras.at(cameraId);
    
    if (!camera->isOpened()) {
        //std::cerr << "ERROR: Could not open camera " << cameraId << std::endl;
        return NULL;
    }

    cv::Mat frame;
    *camera >> frame;

    vector<int> markerIds;
    vector<vector<Point2f>> markerCorners;

    detectMarkers(frame, dictionary, markerCorners, markerIds);

    if (markerIds.empty()) {
        return NULL;
    }

    result = (jobjectArray)env->NewObjectArray((jsize)markerIds.size(), 
        env->FindClass("java/lang/String"), 
        env->NewStringUTF(""));

    for (int i = 0; i < markerIds.size(); i++) {
        char str[255];
        vector<Point2f> corners = markerCorners.at(i);
        int x = (int)((corners.at(0).x + corners.at(2).x) / 2);
        int y = (int)((corners.at(0).y + corners.at(2).y) / 2);
        sprintf_s(str, "%d %d %d", markerIds.at(i), x, y);

        env->SetObjectArrayElement(
            result, i, env->NewStringUTF(str));
    }
    return result;
}