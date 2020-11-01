//
// Created by chengyh23 on 2020/10/31.
//
#ifndef BALL_DETECTOR_COLOR_H
#define BALL_DETECTOR_COLOR_H
#include "opencv2/opencv.hpp"
using namespace cv;
enum COLOR{YELLOW,GREEN,BLACK,RED,UNDEFINED};
double thR=0.4;
double thY=0.85;
double thG=0.3;
double thB=0.3;

//// 根据单点像素RGB值判断颜色
//COLOR colorClassify(int r,int g, int b);

// 根据单张图片各像素颜色统计数值判断该帧颜色
COLOR colorDetect(Mat img);
#endif //BALL_DETECTOR_COLOR_H
