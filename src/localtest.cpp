//
// Created by chengyh23 on 2020/11/1.
//
#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include "../include/color.h"
#include "../include/seaskyline.h"
#include "../include/shape.h"
using namespace std;

int main(int argc,char* argv[])
{
    if(argc<2){
        cout<<"input local frames path directory"<<endl;
        return 0;
    }
    string filePath = argv[1];
    ifstream file;
    file.open(filePath+"imgs.txt",ios::in);

    if(!file.is_open()){
        cout<<"no input imgs list"<<endl;
        return 0;
    }

    std::string imgname;
    while(getline(file,imgname))
    {
        if(imgname.empty())
            continue;
        cv::Mat img = cv::imread(filePath+imgname);
        cout<<imgname;
        if(img.empty()){
            cout<<" failed to read / empty"<<endl;
            break;
        }else{
            cout<<endl;
        }
        // 海天线检测
        cv::Point h1(0, 0), h2(0, 0);
        find_horinzon_line(img, 0, 0.0, 2.0, h1, h2);
        cv::line(img,h1,h2,Scalar(255,0,0),2,cv::LINE_AA);

        std::vector<std::vector<Point>> colorList = colorDetect(img);
        for(int i=0;i<colorList.size();i++){
            cv::Rect roi_rect = drawColorCirclesRect(img,colorList[i],COLOR(i));
//            if(!roi_rect.empty()){
//                cv::Mat roi = img(roi_rect);
//                circleDetect(roi);
//            }
            drawBlockColorCircle(img,colorList[i],COLOR(i));
        }
        cv::Mat tmp;
        cv::resize(img,tmp,cv::Size (1280,720));

        namedWindow("position",0);
        imshow("position",tmp);waitKey(0);// 毫秒
    }
}