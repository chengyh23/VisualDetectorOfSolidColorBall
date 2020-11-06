//
// Created by chengyh23 on 2020/10/31.

//
#include "../include/color.h"
#include "kmeans_minibatch.h"
#include <iostream>
#include <algorithm>


CvSize GRID_SIZE = cvSize(GRID_WIDTH,GRID_HEIGHT);

bool inRange(int x,int min,int max){
    return (x<max) && (x>min);
}
int getMaxMin(vector<int> x,int& max,int& min,int& maxIdx,int& minIdx){
    if(!x.size()>0) return -1;
    max=min=x[0];
    maxIdx=minIdx=0;
    for(int i=0;i<x.size();i++){
        if(x[i]>max) {max = x[i];maxIdx=i;}
        if(x[i]<min) {min = x[i];minIdx=i;}
    }
    return 0;
}
Point getCentroid(std::vector<Point> listOfPoints){
    Data *d = new Data(listOfPoints.size(),2);
    d->loadData(listOfPoints);
    int K=2;
    MiniBatchKmeans *mbk = new MiniBatchKmeans(d, K, MAX_ITER, BATCH_SIZE);
    //mbk->check();
    mbk->initCentroids();
    Point centroid = mbk->fit(MAX_NO_IMPROVEMENT);
    delete d;
    delete mbk;
    Point ret;
    ret.x = (int)(centroid.x * GRID_WIDTH);
    ret.y = (int)(centroid.y * GRID_HEIGHT);
    return ret;
}

// 根据单点像素RGB值判断颜色
COLOR colorClassify(int r,int g, int b){
    int sum=r+g+b;
    if(sum==0) return UNDEFINED;
    float ratioR=(float)r/sum;
    float ratioG=(float)g/sum;
    float ratioB=(float)b/sum;
    float ratioY=(float)(r+g)/sum;

    if((ratioR>=thR) && (ratioG<=thG) && (ratioB<=thB)) return RED;
    if(ratioY >= thY && ratioB<=thB) return YELLOW;
    if(inRange(r,12-10*2,12+10*2)&&
       inRange(g,60-52*2,60+52*2)&&
       inRange(b,66-40*2,66+40*2)) return GREEN;
    if(r<30 && g<30 && b<30) return BLACK;
        // if((ratioR>=thR) && (ratioG<=thG) && (ratioB>=thB)) return PURPLE;
    else return UNDEFINED;
}

// 每个block：像素点歌数最多的颜色作为返回值
COLOR colorDetectBlock(Mat img,int offset_col,int offset_row){
    int nYGBR[4]= {0,0,0,0};
//    Rect x(offset_col,offset_row,GRID_WIDTH,GRID_HEIGHT);
//    rectangle(img,x,Scalar(100,100,100));
//    imshow("rect",img);waitKey(0);
    for(int j=offset_row;j<offset_row+GRID_HEIGHT;j++){      //忽略图像底部
        for(int i=offset_col;i<offset_col+GRID_WIDTH;i++){
            Vec3b pixel;
            pixel=img.at<Vec3b>(j,i);
            COLOR color=colorClassify((int)pixel[2],(int)pixel[1],(int)pixel[0]);
            if(color==UNDEFINED) continue;
            nYGBR[color]++;
        }
    }
//    printf("Y %5d    G %5d   B %5d   R %5d.\n",nYGBR[0],nYGBR[1],nYGBR[2],nYGBR[3]);
    int max=0;int ret=4;
    for(int i=0;i<4;i++){
        if(nYGBR[i]>max){
            max=nYGBR[i];
            ret=i;
        }
    }
    // 这个GRID中不到1/3的像素点为这个颜色，则认为结果不对
    if(max < (int)(GRID_HEIGHT*GRID_WIDTH/3.0)) ret = 4;
    switch (ret) {
        case 0:return YELLOW;break;
        case 1:return GREEN;break;
        case 2:return BLACK;break;
        case 3:return RED;break;
        default:return UNDEFINED;
    }
}
cv::Rect drawColorCirclesRect(Mat& img,const std::vector<cv::Point> colorList,COLOR c){
    if(colorList.size()<NUM_GRIDS_THRESH) return cv::Rect();
    std::vector<cv::Point> points=colorList;
    std::vector<int> px,py;
    for(int ii=0;ii<points.size();ii++){
        px.push_back(points[ii].x);
        py.push_back(points[ii].y);
    }
    // 去掉NUM_OUTIERS个最大最小值
    for(int i=0;i<NUM_OUTLIERS;i++){
        int px_max, px_min, px_maxIdx, px_minIdx;
        getMaxMin(px,px_max,px_min,px_maxIdx, px_minIdx);
        px.erase(px.begin()+px_maxIdx);
        px.erase(px.begin()+px_minIdx);
        int py_max, py_min, py_maxIdx, py_minIdx;
        getMaxMin(py,py_max,py_min,py_maxIdx, py_minIdx);
        py.erase(py.begin()+py_maxIdx);
        py.erase(py.begin()+py_minIdx);
    }
    int px_max, px_min, px_maxIdx, px_minIdx;
    getMaxMin(px,px_max,px_min,px_maxIdx, px_minIdx);
    int py_max, py_min, py_maxIdx, py_minIdx;
    getMaxMin(py,py_max,py_min,py_maxIdx, py_minIdx);
    // 点是GRID左上角坐标，目标框应包含至右下角
    px_max +=1;
    py_max +=1;
    Point p1(px_min*GRID_WIDTH,py_min*GRID_HEIGHT); // topleft
    Point p2(px_max*GRID_WIDTH,py_max*GRID_HEIGHT); // bottomright
    // rectangle如果长宽比太大认为它不是球，过滤掉
    double aspect_ratio=((px_max-px_min)/(py_max-py_min));
    aspect_ratio = (aspect_ratio)>1 ? aspect_ratio : (1/aspect_ratio);
    if(aspect_ratio < ASPECTRATIO_THRESH){
        switch (c){
            case 0://YELLOW
                cv::rectangle(img,p1,p2,Scalar(100,100,100),1,LINE_AA);break;
            case 1://GREEN
                cv::rectangle(img,p1,p2,Scalar(0,255,0),1,LINE_AA);break;
            case 2://BLACK
                cv::rectangle(img,p1,p2,Scalar(0,0,0),1,LINE_AA);break;
            case 3://RED
                cv::rectangle(img,p1,p2,Scalar(0,0,255),1,LINE_AA);break;
        }
        return cv::Rect(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
    }
    return cv::Rect();
}
void drawBlockColorCircle(Mat& img,const std::vector<cv::Point> colorList,COLOR c){
    for(int i=0;i<colorList.size();i++){
        int ii=colorList[i].x;
        int jj=colorList[i].y;
        switch (c){
            case 0://YELLOW
                circle(img,cv::Point(ii*GRID_WIDTH,jj*GRID_HEIGHT),10,Scalar(100,100,100),2);break;
            case 1://GREEN
                circle(img,cv::Point(ii*GRID_WIDTH,jj*GRID_HEIGHT),10,Scalar(0,255,0),2);break;
            case 2://BLACK
                circle(img,cv::Point(ii*GRID_WIDTH,jj*GRID_HEIGHT),10,Scalar(0,0,0),2);break;
            case 3://RED
                circle(img,cv::Point(ii*GRID_WIDTH,jj*GRID_HEIGHT),10,Scalar(0,0,255),2);break;
        }
    }
}
std::vector<std::vector<Point>> colorDetect(Mat& img){
    // YGBR
    std::vector<std::vector<Point>> colorList(4);
    for(int jj=GRID_MARGIN_HEIGHT;jj<GRID_NUMROWS-GRID_MARGIN_HEIGHT;jj++){
        for(int ii=GRID_MARGIN_WIDTH;ii<GRID_NUMCOLS-GRID_MARGIN_WIDTH;ii++){
            COLOR c = colorDetectBlock(img,ii*GRID_WIDTH,jj*GRID_HEIGHT);
            // COLOR为UNDEFINED
            if(c==4) continue;
            colorList[c].push_back(Point(ii,jj));
        }
    }
    /*
    // 各block点聚类取中心
    for(int i=0;i<colorList.size();i++){
        if(colorList[i].size()<10) continue;
        Point centroid = getCentroid(colorList[i]);
        switch (i){
            case 0://YELLOW
                circle(img,centroid,5,Scalar(100,100,100));break;
            case 1://GREEN
                circle(img,centroid,5,Scalar(0,255,0),5);break;
            case 2://BLACK
                circle(img,centroid,5,Scalar(0,0,0));break;
            case 3://RED
                circle(img,centroid,5,Scalar(0,0,255),5);break;
        }
//        CvFont font;
//        cvInitFont(&font,CV_FONT_BLACK,1.0F,1.0F);
//         IplImage ipl = IplImage(img);
//         cvPutText(&ipl,"BBB",centroid,&font,Scalar(0,0,255));
    }
     */

    return colorList;
}


