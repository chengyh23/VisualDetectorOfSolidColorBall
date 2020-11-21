#ifndef SEASKYLINE_H
#define SEASKYLINE_H
#include "../include/params.h"
#include <opencv2/opencv.hpp>
#include <math.h>
#include <vector>

struct SLine
{
    SLine():
        numOfValidPoints(0),
        params(-1.f, -1.f, -1.f, -1.f)
    {}
    cv::Vec4f params;//(cos(t), sin(t), X0, Y0)
    int numOfValidPoints;
};
float get_line_y(SLine param, float x);

cv::Vec4f TotalLeastSquares(std::vector<cv::Point>& nzPoints, std::vector<int> ptOnLine);
SLine LineFitRANSAC(
        float t,//distance from main line
        float p,//chance of hitting a valid pair
        float e,//percentage of outliers
        int T,//number of expected minimum inliers
        std::vector<cv::Point>& nzPoints);

void edgeEnhance(cv::Mat& srcImg, cv::Mat& dstImg);

double line_angle(double x1, double y1, double x2, double y2);

double linear_fit(double x1, double y1, double x2, double y2, double x3);
void find_horinzon_line(cv::Mat img, int start_row, double pre_angle, double angle_thr, cv::Point &p1,
                                              cv::Point &p2);

std::vector<cv::Point> filterByLine(cv::Point h1,cv::Point h2,std::vector<cv::Point> points);
#endif //SEASKYLINE_H
