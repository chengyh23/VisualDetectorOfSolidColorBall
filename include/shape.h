#ifndef HIKVISION_ROS_SHAPE_H
#define HIKVISION_ROS_SHAPE_H
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
/* param for bilateral filter */
int SIGMACOLOR = 200;
int SIGMASPACE = 200;
/* param for morph */
int SZ = 3;
/* param for Canny*/
int THRESHOLD1 = 200;
int THRESHOLD2 = 300;
/* params for HoughCircle() */
int DP = 2;
int MINDIST = 300;
int PARAM1 = 300;
int PARAM2 = 10;

bool SHOW_ROI_HSV   = false;
bool SHOW_ROI_EDGES = false;

std::vector<cv::Vec3f> circleDetect(const cv::Mat& img, const cv::Rect roi_rect);
void drawShapeCircle(cv::Mat& img, const std::vector<std::vector<cv::Vec3f>>& circles);
#endif //HIKVISION_ROS_SHAPE_H
