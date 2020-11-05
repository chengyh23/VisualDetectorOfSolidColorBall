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
//// 根据单点像素RGB值判断颜色
//COLOR colorClassify(int r,int g, int b);
int getMaxMin(vector<int> x,int& max,int& min,int& maxIdx,int& minIdx);

cv::Rect drawColorCirclesRect(Mat& img,const std::vector<cv::Point> colorList,COLOR c);
void drawBlockColorCircle(Mat& img,const std::vector<cv::Point> colorList,COLOR c);
// 根据单张图片各像素颜色统计数值判断该帧颜色
std::vector<std::vector<Point>> colorDetect(Mat& img);
#endif //BALL_DETECTOR_COLOR_H
