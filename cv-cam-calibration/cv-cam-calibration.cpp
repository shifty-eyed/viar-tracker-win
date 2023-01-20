
#include <iostream>
#include <fstream> 
#include <filesystem> 
#include <math.h>
#include <stdio.h>
#include <time.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/calib3d.hpp"

using namespace std;
using namespace cv;

const float squareEdgeSize = 0.02514f;
const float arocoSquareSide = 0.034f;
const Size chessboardSize(9, 6);

const Mat cameraMartix1 = (Mat_<double>(3, 3) << 3261.88, 0, 94.3804, 0, 3047.15, -16.2852, 0, 0, 1);
const Mat distCoefficients1 = (Mat_<double>(5, 1) << -0.250086, 0.244995, -0.0112522, -0.0157054, -0.144077);

int showWebCameraContent(int camId) {

    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_4X4_100);

    VideoCapture camera(camId);
    camera.set(CAP_PROP_FRAME_WIDTH, 1920); camera.set(CAP_PROP_FRAME_HEIGHT, 1080);

    if (!camera.isOpened()) {
        cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }

    namedWindow("Webcam", WINDOW_KEEPRATIO);

    Mat frame;


    clock_t t = clock();
    int frameCount = 0;
    int fps = 0;
    while (1) {
        camera >> frame;

        vector<int> markerIds;
        vector<Vec3d> rotVectors, transVectors;
        vector<vector<Point2f>> markerCorners;
        aruco::detectMarkers(frame, dictionary, markerCorners, markerIds);
        aruco::drawDetectedMarkers(frame, markerCorners, markerIds);
        //aruco::estimatePoseSingleMarkers(markerCorners, arocoSquareSide, cameraMartix1, distCoefficients1, rotVectors, transVectors);
        cv::imshow("Webcam", frame);

        if (clock() - t > CLOCKS_PER_SEC) {
            t = clock();
            fps = frameCount;
            frameCount = 0;
            printf("fps: %d\n", fps);
        }
        frameCount++;
        char key = (char)cv::waitKey(1);
        if (key != -1) {
            break;
        }
    }
    return 0;
}

void generateArucoMarkers() {
    Mat markerImage;
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_4X4_100);
    char filename[50];
    for (int i = 0; i < 10; i++) {
        aruco::drawMarker(dictionary, i, 128, markerImage, 1);
        sprintf_s(filename, "img/marker-%02d.png", i);
        imwrite(filename, markerImage);
    }
}

void createIdealBoardPosition(vector<Point3f>& corners) {
    for (int y = 0; y < chessboardSize.height; y++)
        for (int x = 0; x < chessboardSize.width; x++)
            corners.push_back(Point3f(x * squareEdgeSize, y * squareEdgeSize, 0.0f));
}

void detectChessboardCorners(vector<Mat> images, vector<vector<Point2f>>& result) {
    for (vector<Mat>::iterator img = images.begin(); img != images.end(); img++) {
        vector<Point2f> points;
        bool found = findChessboardCorners(*img, chessboardSize, points, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
        if (found) {
            result.push_back(points);
        }
        //drawChessboardCorners(*img, chessboardSize, points, found);
    }
}

void calibrate(vector<Mat> images, Mat& cameraMatrix, Mat& distanceCoefficients) {

    vector<vector<Point2f>> boardImageSpacePoints;
    detectChessboardCorners(images, boardImageSpacePoints);

    vector<vector<Point3f>> worldSpacePoints(1);
    createIdealBoardPosition(worldSpacePoints[0]);
    worldSpacePoints.resize(boardImageSpacePoints.size(), worldSpacePoints[0]);

    vector<Mat> radialVectors, tangentVectors;
    distanceCoefficients = Mat::zeros(8, 1, CV_64F);

    cv::calibrateCamera(worldSpacePoints, boardImageSpacePoints, chessboardSize,
        cameraMatrix, distanceCoefficients, radialVectors, tangentVectors);
}

void dumpMatrix(ofstream &outFile, Mat matrix) {
    for (int r = 0; r < matrix.rows; r++) {
        for (int c = 0; c < matrix.cols; c++) {
            outFile << matrix.at<double>(r, c) << ", ";
        }
        outFile << endl;
    }
}

bool processAndSaveCalibrationData(string inputImageFilePrefix, string outFileName) {
    vector<Mat> inputImages;
    char inputImageFileName[255];

    cout << "Loading Images" << endl;
    int i = 0;
    for (;;) {
        sprintf_s(inputImageFileName, "%s%02d.jpg", inputImageFilePrefix.c_str(), i++);
        if (!filesystem::exists(inputImageFileName)) {
            break;
        }
        inputImages.push_back(imread(inputImageFileName));
    }

    cout << "Calibrating" << endl;
    Mat cameraMatrix, distanceCoefficients;

    calibrate(inputImages, cameraMatrix, distanceCoefficients);

    cout << "Saving" << endl;
    ofstream outFile(outFileName);
    if (!outFile)
        return false;

    outFile << "// cameraMatrix" << endl;
    dumpMatrix(outFile, cameraMatrix);
    outFile << "// distanceCoefficients" << endl;
    dumpMatrix(outFile, distanceCoefficients);

    outFile.close();
    return true;
}

int calibrationCapture(int cameraId) {
    Mat frame;
    Mat drawFrame;
    vector<Mat> savedImages;

    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
    Mat distanceCoeff;
    vector<vector<Point2f>> markerCorners;
    char filename[50];
    int fileCounter = 0;

    VideoCapture camera(cameraId);
    camera.set(CAP_PROP_FRAME_WIDTH, 1920); camera.set(CAP_PROP_FRAME_HEIGHT, 1080);

    if (!camera.isOpened()) {
        cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }

    namedWindow("CornersTest", WINDOW_AUTOSIZE);
    for (;;) {
        camera >> frame;
        frame.copyTo(drawFrame);
        vector<Vec2f> points;
        bool found = findChessboardCorners(frame, chessboardSize, points, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK);
        drawChessboardCorners(drawFrame, chessboardSize, points, found);
        imshow("CornersTest", drawFrame);
        char key = (char)cv::waitKey(1);
        if (key == 13) {
            sprintf_s(filename, "d:/v/calib-%02d.png", fileCounter++);
            cout << "Savin file: " << filename << std::endl;
            imwrite(filename, frame);
        }
        else if (key == 27) {
            break;
        }
    }
    return 0;

}

int main(int argc, char* argv[]) {
    int cam = 0;//atoi(argv[1]);
    cout << "opening cam " << cam << std::endl;
    
    showWebCameraContent(cam);
    //return calibrationCapture(cam);
    //return processAndSaveCalibrationData("d:/v/set2/calib-", "camera-calib2.txt");

}