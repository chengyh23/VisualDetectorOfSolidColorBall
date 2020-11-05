#include"../include/seaskyline.h"
cv::Vec4f TotalLeastSquares(
        std::vector<cv::Point>& nzPoints,
        std::vector<int> ptOnLine)
{
    //if there are enough inliers calculate model
    float x = 0, y = 0, x2 = 0, y2 = 0, xy = 0, w = 0;
    float dx2, dy2, dxy;
    float t;
    for( size_t i = 0; i < nzPoints.size(); ++i )
    {
        x += ptOnLine[i] * nzPoints[i].x;
        y += ptOnLine[i] * nzPoints[i].y;
        x2 += ptOnLine[i] * nzPoints[i].x * nzPoints[i].x;
        y2 += ptOnLine[i] * nzPoints[i].y * nzPoints[i].y;
        xy += ptOnLine[i] * nzPoints[i].x * nzPoints[i].y;
        w += ptOnLine[i];
    }

    x /= w;
    y /= w;
    x2 /= w;
    y2 /= w;
    xy /= w;

    //Covariance matrix
    dx2 = x2 - x * x;
    dy2 = y2 - y * y;
    dxy = xy - x * y;

    t = (float) atan2( 2 * dxy, dx2 - dy2 ) / 2;
    cv::Vec4f line;
    line[0] = (float) cos( t );
    line[1] = (float) sin( t );

    line[2] = (float) x;
    line[3] = (float) y;

    return line;
}
SLine LineFitRANSAC(
        float t,//distance from main line
        float p,//chance of hitting a valid pair
        float e,//percentage of outliers
        int T,//number of expected minimum inliers
        std::vector<cv::Point>& nzPoints)
{
    int s = 2;//number of points required by the model
    int N = (int)ceilf(log(1-p)/log(1 - pow(1-e, s)));//number of independent trials

    std::vector<SLine> lineCandidates;
    std::vector<int> ptOnLine(nzPoints.size());//is inlier
    cv::RNG rng((uint64)-1);
    SLine line;
    for (int i = 0; i < N; i++)
    {
        //pick two points
        int idx1 = (int)rng.uniform(0, (int)nzPoints.size());
        int idx2 = (int)rng.uniform(0, (int)nzPoints.size());
        cv::Point p1 = nzPoints[idx1];
        cv::Point p2 = nzPoints[idx2];

        //points too close - discard
        if (cv::norm(p1- p2) < t)
        {
            continue;
        }

        //line equation ->  (y1 - y2)X + (x2 - x1)Y + x1y2 - x2y1 = 0
        float a = static_cast<float>(p1.y - p2.y);
        float b = static_cast<float>(p2.x - p1.x);
        float c = static_cast<float>(p1.x*p2.y - p2.x*p1.y);
        //normalize them
        float scale = 1.f/sqrt(a*a + b*b);
        a *= scale;
        b *= scale;
        c *= scale;

        //count inliers
        int numOfInliers = 0;
        for (size_t j = 0; j < nzPoints.size(); ++j)
        {
            cv::Point& p0 = nzPoints[j];
            float rho      = abs(a*p0.x + b*p0.y + c);
            bool isInlier  = rho  < t;
            if ( isInlier ) numOfInliers++;
            ptOnLine[j]    = isInlier;
        }

        if ( numOfInliers < T)
        {
            continue;
        }

        line.params = TotalLeastSquares( nzPoints, ptOnLine);
        line.numOfValidPoints = numOfInliers;
        lineCandidates.push_back(line);
    }

    int bestLineIdx = 0;
    int bestLineScore = 0;
    for (size_t i = 0; i < lineCandidates.size(); i++)
    {
        if (lineCandidates[i].numOfValidPoints > bestLineScore)
        {
            bestLineIdx = i;
            bestLineScore = lineCandidates[i].numOfValidPoints;
        }
    }

    if ( lineCandidates.empty() )
    {
        return SLine();
    }
    else
    {
        return lineCandidates[bestLineIdx];
    }
}

void edgeEnhance(cv::Mat& srcImg, cv::Mat& dstImg)
{
    if (!dstImg.empty())
    {
        dstImg.release();
    }

    std::vector<cv::Mat> rgb;

    if (srcImg.channels() == 3)        // rgb image
    {
        cv::split(srcImg, rgb);
    }
    else if (srcImg.channels() == 1)   // gray image
    {
        rgb.push_back(srcImg);
    }

    // 分别对R、G、B三个通道进行边缘增强
    for (size_t i = 0; i < rgb.size(); i++)
    {
        cv::Mat sharpMat8U;
        cv::Mat sharpMat;
        cv::Mat blurMat;

        // 高斯平滑
        //cv::GaussianBlur(rgb[i], blurMat, cv::Size(3,3), 0, 0);

        // 计算拉普拉斯
        cv::Laplacian(rgb[i], sharpMat, CV_16S);

        // 转换类型}
        sharpMat.convertTo(sharpMat8U, CV_8U);
        cv::add(rgb[i], sharpMat8U, rgb[i]);
    }

    cv::merge(rgb, dstImg);
}

double line_angle(double x1, double y1, double x2, double y2) {
    if(x2-x1 == 0.0) return 90.0;
    return atan(double(y2 - y1) / double(x2 - x1)) * 180.0 / 3.1415926;
}
float get_line_y(SLine param, float x) {
    if(param.params[0] == 0)
        param.params[0] = fmax(0.00001f, param.params[0]);
    float k = param.params[1] / param.params[0];
    return k * (x - param.params[2]) + param.params[3];
}
double linear_fit(double x1, double y1, double x2, double y2, double x3) {
    if(x2 - x1 == 0)
        return y1;

    double a = (y2 - y1) / (x2 - x1);
    return a * (x3 - x1) + y1;
}
void find_horinzon_line(cv::Mat img, int start_row, double pre_angle, double angle_thr, cv::Point &p1,
                                              cv::Point &p2)
{
    cv::Mat grey_img, re_img0,re_img;
    cv::cvtColor(img, grey_img, CV_BGR2GRAY);
    double scale = 0.5;
    cv::resize(grey_img, re_img0, cv::Size(), scale, scale, cv::INTER_LINEAR);
    edgeEnhance(re_img0, re_img);

    // Edge detection
    std::vector<cv::Vec4i> linesP, valid_linesP;
    cv::Ptr<cv::LineSegmentDetector> ls = cv::createLineSegmentDetector(cv::LSD_REFINE_NONE);
    ls->detect(re_img, linesP);

    // delete invalid lines
    std::vector<cv::Point> line_Pts;
    line_Pts.reserve(re_img.cols*2);
    for(auto l:linesP)
    {
        double cur_angle = line_angle(double(l[0]), double(l[1]), double(l[2]), double(l[3]));

        if(fabs(cur_angle - pre_angle) > angle_thr*1.5) continue;
        valid_linesP.emplace_back(l);

        //get line points
        int num_pt = std::max(abs(l[2] - l[0]) * 200 / re_img.cols, 0);
        if(num_pt<2) continue;
        double dx = fabs(l[2] - l[0]) / double(num_pt);
        double x0 = fmin(l[0], l[2]);
        for (int j = 0; j < num_pt; ++j)
        {
            double x = dx * (double)j + x0;
            line_Pts.push_back(cv::Point((int)x, linear_fit(double(l[0]), double(l[1]), double(l[2]), double(l[3]), x)));
        }
    }

    //debug
//    cv::Mat show_line;
//    cv::cvtColor(re_img, show_line, CV_GRAY2BGR);
//    for (int i = 0; i < line_Pts.size(); ++i)
//    {
//        cv::circle(show_line, line_Pts[i], 2, cv::Scalar(0,255,0));
//    }
//    cv::imshow("show_line", show_line);
//    cv::waitKey(0);
    //debug

    //fit line with ransac
    SLine horizon_line = LineFitRANSAC((float)2.0f, 0.99, 0.8, 50, line_Pts);
    cv::Point tp1(0, get_line_y(horizon_line, 0.0f) / (float)scale);
    cv::Point tp2((float)img.cols, get_line_y(horizon_line, (float)re_img.cols) / (float)scale);
    //check validation
    double hor_angle = line_angle(double(tp1.x),double(tp1.y),double(tp2.x),double(tp2.y));
    if(fabs(hor_angle - pre_angle) > angle_thr) return;
    p1=tp1;
    p2=tp2;
    p1.y = tp1.y + start_row;
    p2.y = tp2.y + start_row;
}