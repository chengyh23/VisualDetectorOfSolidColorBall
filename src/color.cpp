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
float computeGridStdErr(Mat img,int offset_col,int offset_row){
    int num_pixels = GRID_HEIGHT*GRID_WIDTH;
    int sumB,sumG,sumR;
    sumB=sumG=sumR=0;
    for(int j=offset_row;j<offset_row+GRID_HEIGHT;j++){      //忽略图像底部
        for(int i=offset_col;i<offset_col+GRID_WIDTH;i++){
            Vec3b pixel;
            pixel=img.at<Vec3b>(j,i);
            sumB+=(int)pixel[0];
            sumG+=(int)pixel[1];
            sumR+=(int)pixel[2];
        }
    }
    float meanB,meanG,meanR;
    meanB = sumB/num_pixels;
    meanG = sumG/num_pixels;
    meanR = sumR/num_pixels;
    float sqrErrSumB,sqrErrSumG,sqrErrSumR;
    sqrErrSumB=sqrErrSumG=sqrErrSumR=0;
    for(int j=offset_row;j<offset_row+GRID_HEIGHT;j++){      //忽略图像底部
        for(int i=offset_col;i<offset_col+GRID_WIDTH;i++){
            Vec3b pixel;
            pixel=img.at<Vec3b>(j,i);
            float errB,errG,errR;
            errB=(int)pixel[0]-meanB; sqrErrSumB+=errB*errB;
            errG=(int)pixel[1]-meanG; sqrErrSumG+=errG*errG;
            errR=(int)pixel[2]-meanR; sqrErrSumR+=errR*errR;
        }
    }
    float stderrB,stderrG,stderrR;
    stderrB = sqrt(sqrErrSumB/num_pixels);
    stderrG = sqrt(sqrErrSumG/num_pixels);
    stderrR = sqrt(sqrErrSumR/num_pixels);
    return stderrB+stderrG+stderrR;
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
    if(inRange(r,35-12*2,35+12*2)&&
       inRange(g,40-9*2,40+9*2)&&
       inRange(b,50-9*2,50+9*2)) return BLACK;
    if(inRange(r,0-2*2,0+2*2)&&
       inRange(g,200-18*2,200+18*2)&&
       inRange(b,80-20*2,80+20*2)&&
       ratioG>0.6) return GREEN;

        // if((ratioR>=thR) && (ratioG<=thG) && (ratioB>=thB)) return PURPLE;
    else return UNDEFINED;
}

// 每个block颜色判断
COLOR colorDetectBlock(Mat img,int offset_col,int offset_row){
    int nYGBR[4]= {0,0,0,0};
    for(int j=offset_row;j<offset_row+GRID_HEIGHT;j++){
        for(int i=offset_col;i<offset_col+GRID_WIDTH;i++){
            Vec3b pixel;
            pixel=img.at<Vec3b>(j,i);
            COLOR color=colorClassify((int)pixel[2],(int)pixel[1],(int)pixel[0]);
            if(color==UNDEFINED) continue;
            nYGBR[color]++;
        }
    }
    int max=0;int ret=4;
    for(int i=0;i<4;i++){
        if(nYGBR[i]>max){
            max=nYGBR[i];
            ret=i;
        }
    }
    // 这个GRID中不到1/3的像素点为这个颜色，则认为结果不对
    if(max < (int)(GRID_HEIGHT*GRID_WIDTH/3.0)) ret = 4;
    // 这个GRID RGB STDERR大于阈值
    float stderr = computeGridStdErr(img, offset_col, offset_row);
    if(stderr > STDERR_THRESH) ret = 4;

    switch (ret) {
        case 0:return YELLOW;break;
        case 1:return GREEN;break;
        case 2:return BLACK;break;
        case 3:return RED;break;
        default:return UNDEFINED;
    }
}

std::vector<cv::Rect> getColorCirclesRect(cv::Point h1,cv::Point h2,const std::vector<std::vector<cv::Point>> colorList){
    std::vector<cv::Rect> ret;
    for(int j=0;j<colorList.size();j++){
        // GRID数量太少则忽略
        if(colorList[j].size()<NUM_GRIDS_THRESH){
            ret.push_back(cv::Rect());
            continue;
        }
        std::vector<cv::Point> points=colorList[j];
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
        Point p1((px_min-2)*GRID_WIDTH,(py_min-2)*GRID_HEIGHT); // topleft
        Point p2((px_max+2)*GRID_WIDTH,(py_max+2)*GRID_HEIGHT); // bottomright
        // rectangle如果长宽比太大认为它不是球，过滤掉
        float aspect_ratio=((float)(px_max-px_min)/(py_max-py_min));
        aspect_ratio = (aspect_ratio)>1 ? aspect_ratio : (1/aspect_ratio);
        // 矩形最上方距离海天线大于图像高度的1/6，过滤
        float dist2seaskyline;
        if(h1.y==0 ||h2.y==0)  dist2seaskyline = -1; // 若未检测到海天线，这条规则自然是用不了的
        else dist2seaskyline= p1.y - (h1.y+h2.y)/2;
        // 矩形最上方如果在图像高度的下1/4之下，过滤
        int dist2bottom=HEIGHT - p1.y;
        if(aspect_ratio < ASPECTRATIO_THRESH && dist2seaskyline<HEIGHT/6 && dist2bottom>HEIGHT/4){
            ret.push_back(cv::Rect(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y));
        }else{
            ret.push_back(cv::Rect());
        }
    }
    return ret;
}
void drawColorCirclesRects(Mat& img,std::vector<cv::Rect> rects){
    for(int c=0;c<rects.size();c++){
        cv::Rect rect = rects[c];
        if(rect==cv::Rect()) continue;
        cv::Point p1(rect.x,rect.y);
        cv::Point p2(rect.x+rect.width,rect.y+rect.height);
        switch (c){
            case 0://YELLOW
                cv::rectangle(img,p1,p2,Scalar(100,100,100),2,LINE_AA);break;
            case 1://GREEN
                cv::rectangle(img,p1,p2,Scalar(0,255,0),2,LINE_AA);break;
            case 2://BLACK
                cv::rectangle(img,p1,p2,Scalar(0,0,0),2,LINE_AA);break;
            case 3://RED
                cv::rectangle(img,p1,p2,Scalar(0,0,255),2,LINE_AA);break;
        }
    }

}
void drawBlockColorCircle(Mat& img,const std::vector<std::vector<cv::Point>> colorList){
    for(int c=0;c<colorList.size();c++){
        for(int i=0;i<colorList[c].size();i++){
            int ii=colorList[c][i].x;
            int jj=colorList[c][i].y;
            switch (c){
                case 0://YELLOW
                    circle(img,cv::Point(ii*GRID_WIDTH,jj*GRID_HEIGHT),6,Scalar(100,100,100),2);break;
                case 1://GREEN
                    circle(img,cv::Point(ii*GRID_WIDTH,jj*GRID_HEIGHT),6,Scalar(0,255,0),2);break;
                case 2://BLACK
                    circle(img,cv::Point(ii*GRID_WIDTH,jj*GRID_HEIGHT),6,Scalar(0,0,0),2);break;
                case 3://RED
                    circle(img,cv::Point(ii*GRID_WIDTH,jj*GRID_HEIGHT),6,Scalar(0,0,255),2);break;
            }
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


