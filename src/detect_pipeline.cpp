//
// Created by chengyh23 on 2020/11/6.
//
#include "../include/detect_pipeline.h"
COLOR detect_pipeline(cv::Mat& img){
    // 海天线检测
    cv::Point h1(0, 0), h2(0, 0);
    find_horinzon_line(img, 0, 0.0, 2.0, h1, h2);// 如果没检测到line，h1和h2都是(0, 0)
    // 检测颜色，返回colorList
    std::vector<std::vector<Point>> colorList = colorDetect(img);
    // 根据海天线过滤colorList，得到colorListFiltered
    std::vector<std::vector<Point>> colorListFilered;
    for(int i=0;i<colorList.size();i++){
        std::vector<Point> colorPoints = filterByLine(h1,h2,colorList[i]);
        colorListFilered.push_back(colorPoints);
    }
    std::vector<cv::Rect> roi_rects = getColorCirclesRect(colorListFilered);
    int maxArea=0;COLOR ret=UNDEFINED;
    std::vector<std::vector<cv::Vec3f>> circles;
    for(int i=0;i<colorListFilered.size();i++){
        cv::Rect roi_rect = roi_rects[i];
        if(roi_rect.area()>maxArea){
            maxArea = roi_rect.area();
            ret = COLOR(i);
        }
        if(SHAPE_CIRCLE_DETECT && !roi_rect.empty()){
            std::vector<cv::Vec3f> circle = circleDetect(img,roi_rect);
            circles.push_back(circle);
        }else{
            //即使没有也要占着vector YGBR位置
            circles.push_back(std::vector<cv::Vec3f>());
        }
    }
    // (draw)visualization
    cv::line(img,h1,h2,Scalar(255,0,0),2,cv::LINE_AA);
    if(DRAW_BLOCK_COLOR_CIRCLE) drawBlockColorCircle(img,colorListFilered);
    drawColorCirclesRects(img,roi_rects);
    if(DRAW_SHAPE_CIRCLE) drawShapeCircle(img,circles);

    return ret;
}