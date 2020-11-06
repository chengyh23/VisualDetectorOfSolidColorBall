// Test shape detect separately
#include "color.h"

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
        for(int i=0;i<10;i++){
            for(int j=0;j<10;j++){
                std::cout<<computeGridStdErr(img,i,j)<<std::endl;
            }
        }
    }

}