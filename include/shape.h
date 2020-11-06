#ifndef HIKVISION_ROS_SHAPE_H
#define HIKVISION_ROS_SHAPE_H
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
/* param for bilateral filter */
int SIGMACOLOR = 20;
int SIGMASPACE = 20;
/* param for morph */
int SZ = 3;
/* param for Canny*/
int THRESHOLD1 = 200;
int THRESHOLD2 = 300;
/* params for HoughCircle() */
int DP = 1;
int MINDIST = 300;
int PARAM1 = 300;
int PARAM2 = 10;

std::vector<cv::Vec3f> circleDetect(const cv::Mat& img);
#endif //HIKVISION_ROS_SHAPE_H
