//
// Created by 狼剩子 on 2017/6/27.
//

#include "armor.h"
#define DEBUG 1

#define CHECK_AREA 0
#define CHECK_AREA_DIV_LENGTH 0

//颜色区分对待
double d_thresholdOnDPanel;

//公用参数
double d_minLightBarArea = 40;
double d_maxLightBarArea = 300;

double d_minAreaDivLength = 1.6;
double d_maxAreaDivLength = 2.5;

double d_minHeightDivWidth = 4;
double d_maxHeightDivWidth = 6;

double d_minHeightDivDistance_small = 0.8;
double d_maxHeightDivDistance_small = 2;

double d_maxDeltaY = 0.3;

double d_maxDeltaHeight = 0.1;

double get_distance(double x1, double y1, double x2, double y2)
{
    return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
}

double ratio(double x1, double y1, double x2, double y2)
{
    return (y2 - y1) / (x2 - x1);
}

Point2f middle(double x1, double y1, double x2, double y2)
{
    return Point2f(0.5*(x1+x2), 0.5*(y1+y2));
}


vector<Point2f> findArmor(Mat m_sourceImage ,int i_color)
{
    vector<Mat> v_srcPanel;
    split(m_sourceImage,v_srcPanel);

    Mat m_Red,m_Green,m_Blue;
    threshold(v_srcPanel[0], m_Blue, 200, 255, CV_THRESH_BINARY);
    threshold(v_srcPanel[1], m_Green, 200, 255, CV_THRESH_BINARY);
    threshold(v_srcPanel[2], m_Red, 200, 255, CV_THRESH_BINARY);

    Mat m_kernelRed = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat m_kernelGreen = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat m_kernelBlue = getStructuringElement(MORPH_RECT, Size(3, 3));

    morphologyEx(m_Red, m_Red, MORPH_OPEN, m_kernelRed);
    morphologyEx(m_Green, m_Green, MORPH_OPEN, m_kernelGreen);
    morphologyEx(m_Blue, m_Blue, MORPH_OPEN, m_kernelBlue);

    Mat m_and;
    bitwise_and(m_Red,m_Blue,m_and);
    bitwise_and(m_and,m_Green,m_and);

    vector<vector<Point> > v_lightBarContours;
    vector<Vec4i> v_lightBarHierarchy;
    findContours( m_and, v_lightBarContours, v_lightBarHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    //面积过滤
    vector<vector<Point> > v_lightBar_suitableArea;
    for (int i=0;i<v_lightBarContours.size();i++)
    {
        double area = contourArea(v_lightBarContours[i]);
        if(CHECK_AREA) {
            if (area > d_minLightBarArea && area < d_maxLightBarArea) {
                v_lightBar_suitableArea.push_back(v_lightBarContours[i]);
            }
        }
        else
            v_lightBar_suitableArea.push_back(v_lightBarContours[i]);
    }

    //面积周长比过滤
    vector<vector<Point> > v_lightBar_suitableAreaDivLength;
    for (int i=0; i<v_lightBar_suitableArea.size(); i++)
    {
        double area = contourArea(v_lightBar_suitableArea[i]);
        double length = arcLength(v_lightBar_suitableArea[i], true);
        double value = area/length;
        if(CHECK_AREA_DIV_LENGTH)
        {
            if (value>d_minAreaDivLength && value<d_maxLightBarArea && CHECK_AREA_DIV_LENGTH)
            v_lightBar_suitableAreaDivLength.push_back(v_lightBar_suitableArea[i]);
        }
        else
            v_lightBar_suitableAreaDivLength.push_back(v_lightBar_suitableArea[i]);
    }

    //寻找最小矩形并用矩形宽高比过滤获取最终的灯条矩形
    vector<RotatedRect> v_lightBarRect;
    for(int i=0;i<v_lightBar_suitableAreaDivLength.size();i++)
    {
        RotatedRect rect = minAreaRect(v_lightBar_suitableAreaDivLength[i]);
        double height = rect.size.height;
        double width = rect.size.width;
        if(height<width){
            double exchange;
            exchange = height;
            height = width;
            width = exchange;
        }
        double d_hDivW = height/width;
        if(d_minHeightDivWidth < d_hDivW && d_hDivW < d_maxHeightDivWidth) {
            v_lightBarRect.push_back(rect);
        }
    }

    //遍历任意两个矩形
    //矩形y坐标差距
    //矩形长边和矩形中心距的关系
    vector<Point2f> target;
    for (int i=0; i<v_lightBarRect.size();i++){
        for (int j=i+1;j<v_lightBarRect.size();j++)
        {
            double height1 = (v_lightBarRect[i].size.height>v_lightBarRect[i].size.width)?
                             v_lightBarRect[i].size.height:
                             v_lightBarRect[i].size.width;
            double height2 = (v_lightBarRect[j].size.height>v_lightBarRect[j].size.width)?
                             v_lightBarRect[j].size.height:
                             v_lightBarRect[j].size.width;
            //判断中心Y偏移量
            double deltaY = abs(v_lightBarRect[i].center.y-v_lightBarRect[j].center.y)/(0.5*(height1+height2));
#if DEBUG
            cout<<"Y偏移"<<deltaY<<endl;
#endif
            bool is_deltaYSuitable = deltaY < d_maxDeltaY;
            //is_deltaYSuitable =1;
            //判断长边的差值比
            double deltaHeight = abs(height1-height2)/(0.5*(height1+height2));
#if DEBUG
            cout<<"长边差值比"<<deltaHeight<<endl;
#endif
            bool is_deltaHSuitable = deltaHeight < d_maxDeltaHeight;


            //判断中心距离和长边的比例
            double distance = get_distance(v_lightBarRect[i].center.x,v_lightBarRect[i].center.y, v_lightBarRect[j].center.x,v_lightBarRect[j].center.y);
            double averageHeight = 0.5*(height1+height2);
            double distanceDivHeight = abs(distance-averageHeight)/averageHeight;
#if DEBUG
            cout<<"高度距离比"<<distanceDivHeight<<endl;
#endif
            bool is_distanceDivHeightSuitable = d_minHeightDivDistance_small<distanceDivHeight && distanceDivHeight<d_maxHeightDivDistance_small;

            if(is_deltaYSuitable
               && is_deltaHSuitable
               && is_distanceDivHeightSuitable
                    ){
                target.push_back(middle(v_lightBarRect[i].center.x,v_lightBarRect[i].center.y,
                                        v_lightBarRect[j].center.x,v_lightBarRect[j].center.y));

            }

        }
    }



#if DEBUG
    Mat m_lightBarContours(Size(640,480),CV_8UC3,Scalar(0,0,0));
    if (v_lightBar_suitableAreaDivLength.size()>0)
    {
        drawContours(m_lightBarContours, v_lightBar_suitableAreaDivLength, -1, Scalar(0,255,255),1);

        for(int i=0;i<v_lightBarRect.size();i++) {
            Point2f P[4];
            v_lightBarRect[i].points(P);
            for (int j = 0; j <= 3; j++) {
                line(m_lightBarContours, P[j], P[(j + 1) % 4], Scalar(0,0,255), 2);
            }
        }

        for (int i=0;i<target.size();i++)
        {
            circle(m_lightBarContours,target[i],5,Scalar(255,255,0),2);
        }

        imshow("lightbarcontours", m_lightBarContours);
    }
    for (int i=0;i<v_lightBar_suitableAreaDivLength.size();i++)
    {
        double area = contourArea(v_lightBar_suitableAreaDivLength[i]);
        double length = arcLength(v_lightBar_suitableAreaDivLength[i], true);
        //cout<<"面积周长比"<<area/length<<endl;
    }

#endif

    imshow("And", m_and);
    return target;
}

vector<Point2f> findArmor_d(Mat m_sourceImage ,int i_color)
{
    vector<Mat> v_srcPanel;
    split(m_sourceImage,v_srcPanel);

    Mat m_Red,m_Green,m_Blue;
    //threshold(v_srcPanel[0], m_Blue, 200, 255, CV_THRESH_BINARY);
    //threshold(v_srcPanel[1], m_Green, 200, 255, CV_THRESH_BINARY);
    //threshold(v_srcPanel[2], m_Red, 200, 255, CV_THRESH_BINARY);
    m_Blue = v_srcPanel[0].clone();
    m_Green = v_srcPanel[1].clone();
    m_Red = v_srcPanel[2].clone();

    Mat m_kernelRed = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat m_kernelGreen = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat m_kernelBlue = getStructuringElement(MORPH_RECT, Size(3, 3));

    morphologyEx(m_Red, m_Red, MORPH_OPEN, m_kernelRed);
    morphologyEx(m_Green, m_Green, MORPH_OPEN, m_kernelGreen);
    morphologyEx(m_Blue, m_Blue, MORPH_OPEN, m_kernelBlue);

    Mat m_dPanel;
    if(i_color == ARMOR_BLUE)
    {
        addWeighted(m_Blue, 1 , m_Green, -1, 0, m_dPanel);
        addWeighted(m_dPanel,1 , m_Red, -1, 0, m_dPanel);
    }
    else{
        addWeighted(m_Red, 1 , m_Green, -0.8, 0, m_dPanel);
        addWeighted(m_dPanel,1 , m_Blue, -0.8, 0, m_dPanel);
        d_thresholdOnDPanel = 10;

    }

    threshold(m_dPanel, m_dPanel, d_thresholdOnDPanel, 255, CV_THRESH_BINARY);
    GaussianBlur(m_dPanel,m_dPanel,Size(5,5),0);
    threshold(m_dPanel, m_dPanel, 100, 255, CV_THRESH_BINARY);
    imshow("delta", m_dPanel);
}

/*
vector<Point2f> findArmor(Mat m_sourceImage ,int i_color)
{
    Mat m_hsvSrc;
    cvtColor(m_sourceImage, m_hsvSrc, COLOR_BGR2HSV);
    imshow("HSV", m_hsvSrc);
    Mat mask;
    Scalar s_blue(255, 0, 0);
    Mat m_lowBlue(m_hsvSrc.size(),CV_8UC3,Scalar(110,180,200));
    Mat m_highBlue(m_hsvSrc.size(),CV_8UC3,Scalar(130,255,255));
    inRange(m_hsvSrc, m_lowBlue, m_highBlue, mask);
    imshow("HSV mask", mask);
}
 */
