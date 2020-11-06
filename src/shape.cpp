//
// Created by chengyh23 on 2020/11/4.
//
#include "../include/shape.h"


std::vector<cv::Vec3f> circleDetect(const cv::Mat& img){
    cv::Mat gray;
    cv::cvtColor(img,gray, CV_BGR2GRAY);
    cv::Mat hsv;
    cv::cvtColor(img,hsv, CV_BGR2HSV);
    cv::Mat opening;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(SZ,SZ));
    cv::morphologyEx(hsv,opening, cv::MORPH_OPEN, kernel);
    cv::Mat bf;
    cv::bilateralFilter(opening,bf, 10, 200, 200);
    cv::imshow("bf",bf);cv::waitKey(0);
    cv::Mat edges;
    cv::Canny(bf,edges, THRESHOLD1, THRESHOLD2);
    cv::imshow("edges",edges);cv::waitKey(0);// 毫秒
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(edges, circles,cv::HOUGH_GRADIENT, DP, MINDIST, PARAM1, PARAM2, edges.cols/10, edges.rows/2);
    // 太靠近roi边框的
    for(int i=0;i<circles.size();i++){
        if(abs(circles[i][0] - img.rows) < img.rows/10 ||
           abs(circles[i][1] - img.cols) < img.cols/10){
            circles.erase(circles.begin()+i);
        }
    }
    return circles;
}


