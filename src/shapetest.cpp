// Test shape detect separately
#include "shape.h"

int main(int argc,char* argv[]){
    if(argc<2){
        std::cout<<"input local frames path directory"<<std::endl;
        return 0;
    }
    std::string filePath = argv[1];
    std::ifstream file;
    file.open(filePath+"imgs.txt",std::ios::in);

    if(!file.is_open()){
        std::cout<<"no input imgs list"<<std::endl;
        return 0;
    }
    std::string imgname;
    while(getline(file,imgname))
    {
        if(imgname.empty())
            continue;
        cv::Mat img = cv::imread(filePath+imgname);
        std::cout<<imgname;
        if(img.empty()){
            std::cout<<" failed to read / empty"<<std::endl;
            break;
        }else{
            std::cout<<std::endl;
        }
        std::vector<cv::Vec3f> circles = circleDetect(img);
        for(int i=0;i<circles.size();i++){
            CvPoint center(circles[i][0],circles[i][1]);
            cv::circle(img,center,circles[i][2],CvScalar(0,0,255),1,cv::LINE_AA,0);
        }
        
        cv::namedWindow("shape",0);
        cv::imshow("shape",img);cv::waitKey(0);// 毫秒
    }
    
}