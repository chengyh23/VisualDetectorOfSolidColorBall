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
#include "../include/detect_pipeline.h"
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
        COLOR c = detect_pipeline(img);
        printf("%d\n",(int)c);
        cv::Mat tmp;
        cv::resize(img,tmp,cv::Size (1280,720));

        namedWindow("position",0);
        imshow("position",tmp);waitKey(0);// 毫秒
    }
}