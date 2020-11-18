//
// Created by chengyh23 on 2020/11/6.
//

#ifndef HIKVISION_ROS_DETECT_PIPELINE_H
#define HIKVISION_ROS_DETECT_PIPELINE_H
#include "../include/color.h"
#include "../include/seaskyline.h"
#include "../include/shape.h"


bool DRAW_BLOCK_COLOR_CIRCLE= false;
bool SHAPE_CIRCLE_DETECT= true;
bool DRAW_SHAPE_CIRCLE= false;



COLOR detect_pipeline(cv::Mat& img);

#endif //HIKVISION_ROS_DETECT_PIPELINE_H
