#include "org_viar_core_CameraProcessor.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include <opencv2/mcc.hpp>

#include "Detector.h"

using namespace std;
using namespace cv;
using namespace cv::aruco;

vector<VideoCapture*> cameras;

Detector detector = Detector();

JNIEXPORT void JNICALL Java_org_viar_core_CameraProcessor_init (JNIEnv* env, jobject instance, jint cameraCount, jboolean serialCameraSelection) {
    for (int cameraId = 0; 
        serialCameraSelection ? cameraId < cameraCount : cameras.size() < cameraCount; 
        cameraId++) {
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
        return NULL;
    }

    cv::Mat frame;
    *camera >> frame;

    vector<ArucoMarkerPosition> arucos;
    vector<BodyPartPosition> bodyParts;

    detector.detectFeaturePositions(frame, arucos, bodyParts);

    if (arucos.empty() && bodyParts.empty()) {
        return NULL;
    }

    result = (jobjectArray)env->NewObjectArray((jsize)arucos.size(),
        env->FindClass("java/lang/String"), 
        env->NewStringUTF(""));

    for (int i = 0; i < arucos.size(); i++) {
        char str[255];
        /*vector<Point2f> corners = markerCorners.at(i);
        int x = (int)((corners.at(0).x + corners.at(2).x) / 2);
        int y = (int)((corners.at(0).y + corners.at(2).y) / 2);
        sprintf_s(str, "%d %d %d", markerIds.at(i), x, y);*/

        env->SetObjectArrayElement(
            result, i, env->NewStringUTF(str));
    }
    return result;
}