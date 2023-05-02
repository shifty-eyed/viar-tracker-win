
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

const float squareEdgeSize = 0.02515f;
const Size chessboardSize(6, 9);

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
    for (int i = 0; i < 20; i++) {
        aruco::drawMarker(dictionary, i, 256, markerImage, 1);
        sprintf_s(filename, "d:/ws/cv-tracker/img/DICT_4X4_100-marker-%02d.png", i);
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

double calibrate(vector<Mat> images, Mat& cameraMatrix, Mat& distCoefficients) {

    vector<vector<Point2f>> boardImageSpacePoints;
    detectChessboardCorners(images, boardImageSpacePoints);

    vector<vector<Point3f>> worldSpacePoints(1);
    createIdealBoardPosition(worldSpacePoints[0]);
    worldSpacePoints.resize(boardImageSpacePoints.size(), worldSpacePoints[0]);

    vector<Mat> rvec, tvec;
    distCoefficients = Mat::zeros(8, 1, CV_64F);

    return cv::calibrateCamera(worldSpacePoints, boardImageSpacePoints, chessboardSize,
        cameraMatrix, distCoefficients, rvec, tvec);
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
        sprintf_s(inputImageFileName, "%s/calib-%02d.png", inputImageFilePrefix.c_str(), i++);
        if (!filesystem::exists(inputImageFileName)) {
            break;
        }
        inputImages.push_back(imread(inputImageFileName));
    }

    cout << "Calibrating" << endl;
    Mat cameraMatrix, distanceCoefficients;

    double error = calibrate(inputImages, cameraMatrix, distanceCoefficients);

    cout << "Saving" << endl;
    ofstream outFile(outFileName);
    if (!outFile)
        return false;

    outFile << "// cameraMatrix" << endl;
    dumpMatrix(outFile, cameraMatrix);
    outFile << "// fx,fy,cx,cy" << endl;
    outFile << cameraMatrix.at<double>(0, 0) << ", " << cameraMatrix.at<double>(1,1) << ", "
        << cameraMatrix.at<double>(0, 2) << ", " << cameraMatrix.at<double>(1, 2)  << endl;

    double fovx, fovy, focalLength, aspectRatio;
    Point2d principalPoint;
    calibrationMatrixValues(cameraMatrix, Size(1920, 1080), 5, 5, fovx, fovy, focalLength, principalPoint, aspectRatio);
    outFile << "fovx=" <<fovx << ", fovy=" << fovy << endl;
    outFile << "// distCoefficients" << endl;
    dumpMatrix(outFile, distanceCoefficients);

    outFile << "error= " << error << endl;
    outFile.close();
    return true;
}

int calibrationCapture(int cameraId, string savePath) {
    Mat frame;
    Mat drawFrame;

    char filename[50];
    int fileCounter = 0;

    VideoCapture camera(cameraId);
    camera.set(CAP_PROP_FRAME_WIDTH, 1920); camera.set(CAP_PROP_FRAME_HEIGHT, 1080);
    camera.set(CAP_PROP_AUTOFOCUS, 0);

    if (!camera.isOpened()) {
        cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }
    
    filesystem::create_directories(savePath);
    namedWindow("CornersTest", WINDOW_AUTOSIZE);
    for (;;) {
        camera >> frame;
        flip(frame, drawFrame, 1);
        vector<Vec2f> points;


        imshow("CornersTest", drawFrame);
        char key = (char)cv::waitKey(1);
        if (key == 13 || key == 32) {
            bool found = findChessboardCorners(frame, chessboardSize, points, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);
            if (found) {
                frame.copyTo(drawFrame);
                drawChessboardCorners(drawFrame, chessboardSize, points, found);
                flip(drawFrame, drawFrame, 1);
                imshow("CornersTest", drawFrame);

                sprintf_s(filename, "%s/calib-%02d.png", savePath.c_str(), fileCounter++);
                cout << "Savin file: " << filename << std::endl;
                imwrite(filename, frame);
                cv::waitKey(700);
            }
        }
        else if (key == 27) {
            destroyAllWindows();
            break;
        }
    }
    return fileCounter;
}

int main(int argc, char* argv[]) {
    int result = 1;
    int cam = 0;//atoi(argv[1]);
    string name = "d:/ws/calib/cam0-set2";
    cout << "opening cam " << cam << std::endl;

    generateArucoMarkers();
    //result = calibrationCapture(cam, name);
    //return result > 0 ? processAndSaveCalibrationData(name, name + ".txt") : result;

}