//
// Created by chengyh23 on 2020/11/4.
//
#include "../include/shape.h"

std::vector<cv::Vec3f> circleDetect(const cv::Mat& img, const cv::Rect roi_rect){
    cv::Mat roi;
    img(roi_rect).copyTo(roi);
//    std::vector<cv::Mat> roichannels;
//    cv::split(roi,roichannels);
//    cv::imshow("roichannels",roichannels[0]);cv::waitKey(0);
//    cv::imshow("roichannels",roichannels[1]);cv::waitKey(0);
//    cv::imshow("roichannels",roichannels[2]);cv::waitKey(0);
    cv::Mat gray;
    cv::cvtColor(roi,gray, CV_BGR2GRAY);
    cv::Mat hsv;
    cv::cvtColor(roi,hsv, CV_BGR2HSV);
    cv::Mat opening_hsv;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(SZ,SZ));
    cv::morphologyEx(hsv,opening_hsv, cv::MORPH_OPEN, kernel);
    cv::Mat bf_hsv;
    cv::bilateralFilter(opening_hsv,bf_hsv, 10, SIGMACOLOR, SIGMASPACE);
    if(SHOW_ROI_HSV){
        cv::imshow("bf",bf_hsv);cv::waitKey(0);
    }
    cv::Mat edges;
    cv::Canny(bf_hsv,edges, THRESHOLD1, THRESHOLD2);
    if(SHOW_ROI_EDGES){
        cv::imshow("edges",edges);cv::waitKey(0);
    }
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(edges, circles,cv::HOUGH_GRADIENT, DP, MINDIST, PARAM1, PARAM2, edges.cols/10, edges.rows/2);
    // 太靠近roi边框的
    for(int i=0;i<circles.size();i++){
        if(abs(circles[i][0] - roi.rows) < roi.rows/10 ||
           abs(circles[i][1] - roi.cols) < roi.cols/10){
            circles.erase(circles.begin()+i);
        }else{
            // ROI相对位置转为img上的绝对位置
            circles[i][0] +=roi_rect.x;
            circles[i][1] +=roi_rect.y;
        }
    }
    return circles;
}
void drawShapeCircle(cv::Mat& img, const std::vector<std::vector<cv::Vec3f>>& circles){
    for(int c=0;c<circles.size();c++){
        std::vector<cv::Vec3f> circle = circles[c];
        for(int i=0;i<circle.size();i++){
            CvPoint center(circle[i][0],circle[i][1]);
            switch (c){
                case 0://YELLOW
                    cv::circle(img,center,circle[i][2],CvScalar(100,100,100),2,cv::LINE_AA,0);break;
                case 1://GREEN
                    cv::circle(img,center,circle[i][2],CvScalar(0,255,0),2,cv::LINE_AA,0);break;
                case 2://BLACK
                    cv::circle(img,center,circle[i][2],CvScalar(0,0,0),2,cv::LINE_AA,0);break;
                case 3://RED
                    cv::circle(img,center,circle[i][2],CvScalar(0,0,255),2,cv::LINE_AA,0);break;
            }
        }
    }
}


