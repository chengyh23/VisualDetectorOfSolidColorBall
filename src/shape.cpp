//
// Created by chengyh23 on 2020/11/4.
//
#include "../include/shape.h"

cv::Scalar lower_green = cv::Scalar(35, 50, 100); // 设定绿色的阈值下限
cv::Scalar upper_green = cv::Scalar(77, 255, 255);  // 设定绿色的阈值上限
void circleDetect(cv::Mat& img){
    cv::Mat gray;
    cv::cvtColor(img,gray, CV_BGR2GRAY);
    cv::Mat hsv;
    cv::cvtColor(img,hsv, CV_BGR2HSV);
    cv::Mat mask;
    cv::inRange(hsv, lower_green, upper_green,mask);
    cv::Mat opening;

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
    cv::morphologyEx(mask,opening, cv::MORPH_OPEN, kernel);
    // cv::bilateralFilter(mask, 10, 200, 200);
    cv::Mat edges;
    cv::Canny(opening,edges, 50, 100);
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(edges, circles,cv::HOUGH_GRADIENT, 1, 100, 100, 10, 10, 500);
    for(int i=0;i<circles.size();i++){
        CvPoint center(circles[i][0],circles[i][1]);
        cv::circle(img,center,circles[i][2],CvScalar(0,0,255));
    }
}


