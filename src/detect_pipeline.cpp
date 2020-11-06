//
// Created by chengyh23 on 2020/11/6.
//
#include "../include/detect_pipeline.h"
void detect_pipeline(cv::Mat& img){
    // 海天线检测
    cv::Point h1(0, 0), h2(0, 0);
    find_horinzon_line(img, 0, 0.0, 2.0, h1, h2);// 如果没检测到line，h1和h2都是(0, 0)
    cv::line(img,h1,h2,Scalar(255,0,0),2,cv::LINE_AA);
    // 检测颜色，返回colorList
    std::vector<std::vector<Point>> colorList = colorDetect(img);
    // 根据海天线过滤colorList，得到colorListFiltered
    std::vector<std::vector<Point>> colorListFilered;
    for(int i=0;i<colorList.size();i++){
        std::vector<Point> colorPoints = filterByLine(h1,h2,colorList[i]);
        colorListFilered.push_back(colorPoints);
    }
    // 利用colorListFiltered
    for(int i=0;i<colorListFilered.size();i++){
        drawBlockColorCircle(img,colorListFilered[i],COLOR(i));
        cv::Rect roi_rect = getColorCirclesRect(colorListFilered[i]);
        drawColorCirclesRect(img,roi_rect,COLOR(i));
        if(CIRCLEDETECT && !roi_rect.empty()){
            cv::Mat roi;
            img(roi_rect).copyTo(roi);
            std::vector<cv::Vec3f> circles = circleDetect(roi);
            for(int i=0;i<circles.size();i++){
                CvPoint center(circles[i][0]+roi_rect.x,circles[i][1]+roi_rect.y);
                cv::circle(img,center,circles[i][2],CvScalar(0,0,255),1,LINE_AA,0);
            }
        }
    }
}