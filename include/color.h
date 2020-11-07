//
// Created by chengyh23 on 2020/10/31.
//
#ifndef BALL_DETECTOR_COLOR_H
#define BALL_DETECTOR_COLOR_H
#include "../include/hik_cam.h"
#include "../include/detect_params.h"
#include "opencv2/opencv.hpp"
using namespace cv;
enum COLOR{YELLOW,GREEN,BLACK,RED,UNDEFINED};
double thR=0.4;
double thY=0.85;
double thG=0.3;
double thB=0.3;
int STDERR_THRESH=100;
//// 根据单点像素RGB值判断颜色
//COLOR colorClassify(int r,int g, int b);
int getMaxMin(vector<int> x,int& max,int& min,int& maxIdx,int& minIdx);
float computeGridStdErr(Mat img,int offset_col,int offset_row);

void drawBlockColorCircle(Mat& img,const std::vector<std::vector<cv::Point>> colorList);

std::vector<cv::Rect> getColorCirclesRect(const std::vector<std::vector<cv::Point>> colorList);
void drawColorCirclesRects(Mat& img,std::vector<cv::Rect> rects);

std::vector<std::vector<Point>> colorDetect(Mat& img);
#endif //BALL_DETECTOR_COLOR_H
