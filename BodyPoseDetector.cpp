#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <chrono>

#include "Stopwatch.h"
#include "BodyPoseDetector.h"


using namespace dnn;
#define RED Scalar(0,0,255)

BodyPoseDetector::BodyPoseDetector(String modelBin, String modelTxt, bool useGPU)
{
	net = readNet(modelBin, modelTxt);

    if (useGPU) {
        net.setPreferableBackend(DNN_BACKEND_CUDA);
        net.setPreferableTarget(DNN_TARGET_CUDA);
    }
}

string BodyPoseDetector::detect(Mat image, vector<BodyPartPosition>& result)
{
    Stopwatch sw;

    sw.start("blobFromImage");
	Mat inputBlob = blobFromImage(image, 0.5, Size(656, 368), Scalar(128, 128, 128));
	net.setInput(inputBlob);

    sw.stop("blobFromImage");


    sw.start("netForward");
	Mat out = net.forward();
    sw.stop("netForward");

    int H = out.size[2];
    int W = out.size[3];

    float SX = float(image.cols) / W;
    float SY = float(image.rows) / H;

    // find the position of the body parts
    vector<Point> points(NUM_PARTS);

    sw.start("heatmap");
    for (int n = 0; n < NUM_PARTS; n++) {
        // Slice heatmap of corresponding body's part.
        Mat heatMap(H, W, CV_32F, out.ptr(0, n));
        // 1 maximum per heatmap
        Point p(-1, -1), pm;
        double conf;
        minMaxLoc(heatMap, 0, &conf, 0, &pm);
        if (conf > THRESHOLD)
            p = pm;
        p.x *= SX;
        p.y *= SY;
        points[n] = p;
    }
    stringstream msg;
    msg << "Heatmap: " << W << "x" << H;

    putText(image, msg.str(), Point(30, 70), FONT_HERSHEY_SIMPLEX, 0.5f, RED, 1);

    sw.stop("heatmap");
    result.push_back({0, points});
    return sw.toString();
}
