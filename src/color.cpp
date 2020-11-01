//
// Created by chengyh23 on 2020/10/31.
//
#include "color.h"
#include "kmeans_minibatch.h"

// 1440/1080 = 4/3
// 40x30
int WIDTH = 1440;
int HEIGHT = 1080;
int GRID_NUMCOLS = 40;
int GRID_NUMROWS = 30;
int GRID_WIDTH = WIDTH/GRID_NUMCOLS;
int GRID_HEIGHT = HEIGHT/GRID_NUMROWS;
CvSize GRID_SIZE = cvSize(GRID_WIDTH,GRID_HEIGHT);

bool inRange(int x,int min,int max){
    return (x<max) && (x>min);
}
Point getCentroid(std::vector<Point> listOfPoints){
    for(int i=0;i<listOfPoints.size();i++){
        Data *d = new Data(listOfPoints.size(),2);
        d->loadData(listOfPoints);
        int K=2;
        MiniBatchKmeans *mbk = new MiniBatchKmeans(d, K, MAX_ITER, BATCH_SIZE);
        //mbk->check();
        mbk->initCentroids();
        mbk->fit(MAX_NO_IMPROVEMENT);
        delete d;
        delete mbk;
    }
}

// 根据单点像素RGB值判断颜色
COLOR colorClassify(int r,int g, int b){
    int sum=r+g+b;
    if(sum==0) return UNDEFINED;
    float ratioR=(float)r/sum;
    float ratioG=(float)g/sum;
    float ratioB=(float)b/sum;
    float ratioY=(float)(r+g)/sum;

    if(ratioY >= thY && ratioB<=thB) return YELLOW;
    if(inRange(r,39-22*2,39+22*2)&&
       inRange(g,140-24*2,140+24*2)&&
       inRange(b,117-24*2,117+24*2)) return GREEN;
    if(r<80 && g<80 && b<80) return BLACK;
    if((ratioR>=thR) && (ratioG<=thG) && (ratioB<=thB)) return RED;
        // if((ratioR>=thR) && (ratioG<=thG) && (ratioB>=thB)) return PURPLE;
    else return UNDEFINED;
}

// 每个block：像素点歌数最多的颜色作为返回值
COLOR colorDetectBlock(Mat img,int offset_col,int offset_row){
    int nYGBR[4]= {0,0,0,0};
//    Rect x(offset_col,offset_row,GRID_WIDTH,GRID_HEIGHT);
//    rectangle(img,x,Scalar(100,100,100));
//    imshow("rect",img);waitKey(0);
    for(int i=offset_row;i<offset_row+GRID_HEIGHT;i++){      //忽略图像底部
        for(int j=0;j<offset_col+GRID_WIDTH;j++){
            Vec3b pixel;
            pixel=img.at<Vec3b>(i,j);
            COLOR color=colorClassify((int)pixel[2],(int)pixel[1],(int)pixel[0]);
            if(color==UNDEFINED) continue;
            nYGBR[color]++;
        }
    }
    // printf("Y %5d    G %5d   B %5d   R %5d    ",nYGBR[0],nYGBR[1],nYGBR[2],nYGBR[3]);
    int max=0;int ret=4;
    for(int i=0;i<4;i++){
        if(nYGBR[i]>max){
            max=nYGBR[i];
            ret=i;
        }
    }
    switch (ret) {
        case 0:return YELLOW;break;
        case 1:return GREEN;break;
        case 2:return BLACK;break;
        case 3:return RED;break;
        default:return UNDEFINED;
    }
}
COLOR colorDetect(Mat img){
    // YGBR
    std::vector<std::vector<Point>> colorList(4);
    for(int ii=0;ii<GRID_NUMCOLS;ii++){
        for(int jj=0;jj<GRID_NUMROWS;jj++){
            COLOR c = colorDetectBlock(img,ii*GRID_WIDTH,jj*GRID_HEIGHT);
//            printf("%d",c);
            // COLOR为UNDEFINED
            if(c==4) continue;
            colorList[c].push_back(Point(ii,jj));
        }
    }
    CvFont font;
    cvInitFont(&font,CV_FONT_BLACK,1.0F,1.0F);
    for(int i=0;i<colorList.size();i++){
        // 没有被标记为该颜色的block
        if(colorList[i].size()==0) continue;
        Point centroid = getCentroid(colorList[i]);
        circle(img,centroid,60,Scalar(0,0,255));
        // IplImage ipl = IplImage(img);
        // cvPutText(&ipl,"BBB",centroid,&font,Scalar(0,0,255));
        imshow("position",img);waitKey(0);
    }
}

