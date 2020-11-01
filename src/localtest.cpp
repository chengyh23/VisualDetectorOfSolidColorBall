//
// Created by chengyh23 on 2020/11/1.
//
#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include "color.h"

using namespace std;

int main(int argc,char* argv[])
{
    if(argc<2){
        cout<<"input local frames path directory"<<endl;
    }
    string filePath = argv[1];
    system("ls |grep .jpg > imgs.txt");
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
        }

        COLOR result = colorDetect(img);
    }
}