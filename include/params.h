//
// Created by chengyh23 on 2020/11/4.
//

#ifndef HIKVISION_ROS_DETECT_PARAMS_H
#define HIKVISION_ROS_DETECT_PARAMS_H
#include <opencv2/opencv.hpp>

// 检测pipeline
bool DRAW_SEASKYLINE= false;//true
bool DRAW_BLOCK_COLOR_CIRCLE= false;
bool DRAW_COLOR_CIRCLES_RECTS= false;//true
bool SHAPE_CIRCLE_DETECT= true;
bool DRAW_SHAPE_CIRCLE= false;

// 图像分辨率
int WIDTH = 1280;
int HEIGHT = 720;
// 图像分成GRID MAP
int GRID_NUMCOLS = 80;
int GRID_NUMROWS = 60;
int GRID_WIDTH = WIDTH/GRID_NUMCOLS;
int GRID_HEIGHT = HEIGHT/GRID_NUMROWS;
// 忽略图像边缘GRID的数量
int GRID_MARGIN_HEIGHT = 10;
int GRID_MARGIN_WIDTH = 5;

int NUM_GRIDS_THRESH = 1;   // RECT含有GRID数量应大于阈值
int NUM_OUTLIERS=0;         // GRID CIRCLE去掉的最大/最小值个数
int ASPECTRATIO_THRESH = 3; // RECT长宽比

/* 在color.cpp中的colorClassify使用 */
double thR=0.4;
double thY=0.9;
double thG=0.3;
double thB=0.3;
// // 11-20晴天
// int BLACK_R_MEAN =58 ; int BLACK_R_STDERR =10;
// int BLACK_G_MEAN =65 ; int BLACK_G_STDERR =8 ;
// int BLACK_B_MEAN =70 ; int BLACK_B_STDERR =8 ;
// int GREEN_R_MEAN =5  ; int GREEN_R_STDERR =10;
// int GREEN_G_MEAN =220; int GREEN_G_STDERR =25;
// int GREEN_B_MEAN =80 ; int GREEN_B_STDERR =20;
//// 11-21多云
//int BLACK_R_MEAN =35 ; int BLACK_R_STDERR =12;
//int BLACK_G_MEAN =40 ; int BLACK_G_STDERR =9 ;
//int BLACK_B_MEAN =50 ; int BLACK_B_STDERR =9 ;
//int GREEN_R_MEAN =0  ; int GREEN_R_STDERR =2 ;
//int GREEN_G_MEAN =200; int GREEN_G_STDERR =18;
//int GREEN_B_MEAN =80 ; int GREEN_B_STDERR =20;
// 11-22大晴天
int BLACK_R_MEAN =75 ; int BLACK_R_STDERR =10;
int BLACK_G_MEAN =100; int BLACK_G_STDERR =10;
int BLACK_B_MEAN =130; int BLACK_B_STDERR =10;
int LIGHT_GREEN_R_MEAN =60 ; int LIGHT_GREEN_R_STDERR =25;
int LIGHT_GREEN_G_MEAN =250; int LIGHT_GREEN_G_STDERR =5;
int LIGHT_GREEN_B_MEAN =150; int LIGHT_GREEN_B_STDERR =25;
int DARK_GREEN_R_MEAN  =70 ; int DARK_GREEN_R_STDERR  =10;
int DARK_GREEN_G_MEAN  =165; int DARK_GREEN_G_STDERR  =15;
int DARK_GREEN_B_MEAN  =115; int DARK_GREEN_B_STDERR  =10;
// int GREEN_R_MEAN  =20 ; int GREEN_R_STDERR  =10;
// int GREEN_G_MEAN  =250; int GREEN_G_STDERR  =10;
// int GREEN_B_MEAN  =150; int GREEN_B_STDERR  =25;

/* 在color.cpp中的colorDetectBlock使用 */
int STDERR_THRESH=10;//100


#endif //HIKVISION_ROS_DETECT_PARAMS_H
