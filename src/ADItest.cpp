// Test ADI separately
#include "opencv2/opencv.hpp"
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
    std::string imgname0;
    getline(file,imgname0);
    if(imgname0.empty()) return 0;
    cv::Mat img0 = cv::imread(filePath+imgname0);
    std::string imgname1;
    while(getline(file,imgname1))
    {
        if(imgname1.empty())
            continue;
        cv::Mat img1 = cv::imread(filePath+imgname1);
        std::cout<<imgname1;
        if(img1.empty()){
            std::cout<<" failed to read / empty"<<std::endl;
            break;
        }else{
            std::cout<<std::endl;
        }
        cv::Mat DI=img1-img0;
        cv::namedWindow("DI",0);
        cv::imshow("DI",DI);cv::waitKey(0);// 毫秒
        img0 = img1.clone();
    }

}