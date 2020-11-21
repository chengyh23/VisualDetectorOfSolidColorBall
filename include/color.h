//
// Created by chengyh23 on 2020/10/31.
//
#ifndef BALL_DETECTOR_COLOR_H
#define BALL_DETECTOR_COLOR_H
#include "../include/hik_cam.h"
#include "../include/params.h"
#include "opencv2/opencv.hpp"
using namespace cv;
enum COLOR{YELLOW,GREEN,BLACK,RED,UNDEFINED};


//// 根据单点像素RGB值判断颜色
//COLOR colorClassify(int r,int g, int b);
int getMaxMin(vector<int> x,int& max,int& min,int& maxIdx,int& minIdx);
float computeGridStdErr(Mat img,int offset_col,int offset_row);

void drawBlockColorCircle(Mat& img,const std::vector<std::vector<cv::Point>> colorList);

std::vector<cv::Rect> getColorCirclesRect(cv::Point h1,cv::Point h2,const std::vector<std::vector<cv::Point>> colorList);
void drawColorCirclesRects(Mat& img,std::vector<cv::Rect> rects);

std::vector<std::vector<Point>> colorDetect(Mat& img);
#endif //BALL_DETECTOR_COLOR_H
