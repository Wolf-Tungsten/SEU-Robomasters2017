
//
// Created by 狼剩子 on 2017/6/27.
//

#include "armor.h"
#define DEBUG 1

int i_cannyHi = 15; //Canny算子上阈值
int i_cannyLo = 45; //Canny算子下阈值
int i_highLight = 215; //灯条原图高阈值
int i_lowLight = 10; //灯条原图低阈值
double k = 1.3;//椭圆比例
double d_dAngle = 5; //灯条斜率差距的阈值
double d_minDistance = 300; //灯条中心距离的阈值
double d_greenRectArea = 2000; //绿通道装甲板面积
double d_dDistanceBetweenLightBarAndGreenRect =100; //灯条和边距间的垂直距离
double d_dAngleBetweenLightBarAndGreenRect = 180;
int i_greenCannyHi = 15;
int i_greenCannyLo = 75;
int dilation_size =3;
int morpho_size = 3;
vector< vector<Point2f> >  v_pointSet;

double distance(double x1,double y1,double x2,double y2){
return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}




vector<Point2f> findArmor(Mat m_sourceImage,int i_color )
{
    resize(m_sourceImage,m_sourceImage,Size(640,480));
    //m_sourceImage.convertTo(m_sourceImage,m_sourceImage.type(),3,-400);

    Mat m_sourcePanel,m_dvalue; //分别存储指定图像对应颜色通道的数组、指定颜色通道做差后的数组
    vector<Mat> panel;
    split(m_sourceImage,panel);
    if(i_color == ARMOR_BLUE)
    {
        m_sourcePanel = panel[0].clone();
        addWeighted(panel[0],2,panel[1],-1,0,m_dvalue);
        addWeighted(m_dvalue,1,panel[2],-1,0,m_dvalue);
        i_highLight = 120;
        dilation_size =0;
        morpho_size = 3;
    }
    else if (i_color == ARMOR_RED)
    {
        m_sourcePanel = panel[2].clone();
        addWeighted(panel[2],2,panel[0],-1,0,m_dvalue);
        addWeighted(m_dvalue,1,panel[1],-1,0,m_dvalue);
        i_highLight = 160;
        dilation_size =5;
        morpho_size = 3;
    }
    else {
        cerr << "Color set Error!" << endl;
        vector<Point2f> empty;
        return empty;
    }
    Mat green_panel = panel[1].clone();
    Mat green_canny;
    equalizeHist(green_panel,green_canny);
    GaussianBlur(green_canny,green_canny,Size(7,7),0);
    Canny(green_canny,green_canny,25,50,3);
    vector<vector<Point> > v_greenContours;
    vector<Vec4i> v_greenHierarchy;
    /// 阈值化检测边界
    //threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
    /// 寻找轮廓
    findContours( green_canny, v_greenContours, v_greenHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    vector<RotatedRect> v_greenMinRect(v_greenContours.size() );
    vector<RotatedRect> v_greenMinEllipse( v_greenContours.size() );

    for( int i = 0; i < v_greenContours.size(); i++ )
    { v_greenMinRect[i] = minAreaRect( Mat(v_greenContours[i]) );
        if(v_greenContours[i].size() > 30 &&
                (v_greenMinRect[i].size.width>v_greenMinRect[i].size.height?(v_greenMinRect[i].size.width*1.0/v_greenMinRect[i].size.height):(v_greenMinRect[i].size.height*1.0/v_greenMinRect[i].size.width))<=k)
        {
            cout<<"椭圆斜率"<<((v_greenMinRect[i].size.width>v_greenMinRect[i].size.height)?(v_greenMinRect[i].size.width*1.0/v_greenMinRect[i].size.height):(v_greenMinRect[i].size.height*1.0/v_greenMinRect[i].size.width))<<endl;


            v_greenMinEllipse[i] = fitEllipse( Mat(v_greenContours[i]) ); }
    }

#if DEBUG
    imshow("green",green_panel);
    imshow("greenCanny",green_canny);
#endif














    Mat m_canny;
    //高斯模糊
    threshold(m_dvalue,m_canny,160,255,0);

    Mat element_dilate = getStructuringElement( MORPH_RECT,
                                         Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                         Point( dilation_size, dilation_size ) );
    Mat element_morpho = getStructuringElement( MORPH_RECT,
                                         Size( 2*morpho_size + 1, 2*morpho_size+1 ),
                                         Point( morpho_size, morpho_size ) );
    /// 膨胀操作
    dilate( m_canny,m_canny , element_dilate );

    morphologyEx(m_canny,m_canny,3,element_morpho);

    //GaussianBlur(m_dvalue,m_canny,Size(3,3),0);
    //Canny算子
    Canny(m_canny,m_canny,i_cannyLo,i_cannyHi);
    //寻找灯条边缘
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( m_canny, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );



    /// 对每个轮廓计算其凸包
    vector<vector<Point> >hull( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
    {
        convexHull( Mat(contours[i]), hull[i], false );
    }

    /// 绘出轮廓及其凸包
#if DEBUG
    Mat drawing = Mat::zeros( m_canny.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        RNG rng(12345);
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        //drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        drawContours( drawing, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
    }
#endif
    ///

    vector<RotatedRect> v_minRect(hull.size());
    vector<RotatedRect> v_lightBar;//存储灯条矩形
    for(int i=0;i<hull.size();i++)
    {
        v_minRect[i]=minAreaRect(hull[i]);
        float f_dpanel,f_srcPanel;
        Mat m_dpanelROI,m_srcPanelROI;
        m_dpanelROI = m_dvalue(Rect((v_minRect[i].center.x-10)<=0||(v_minRect[i].center.x+10)>=640?0:v_minRect[i].center.x-10,(v_minRect[i].center.y-10)<=0||(v_minRect[i].center.y+10)>=480?0:v_minRect[i].center.y-10,20,20));
        m_srcPanelROI = m_sourcePanel(Rect((v_minRect[i].center.x-10)<=0||(v_minRect[i].center.x+10)>=640?0:v_minRect[i].center.x-10,(v_minRect[i].center.y-10)<=0||(v_minRect[i].center.y+10)>=480?0:v_minRect[i].center.y-10,20,20));

        f_dpanel = mean(m_dpanelROI)[0];
        f_srcPanel = mean(m_srcPanelROI)[0];
        cout<<"减色："<<f_dpanel<<" 原通道："<<f_srcPanel<<endl;
        if(
                //f_srcPanel>i_highLight &&
            f_srcPanel-f_dpanel>100

                ) {

            v_lightBar.push_back(v_minRect[i]);
#if DEBUG
            Point2f Drect[4];
            v_minRect[i].points(Drect);
            for (int j = 0; j < 4; j++)
                line(drawing, Drect[j], Drect[(j + 1) % 4], Scalar(0, 255, 255), 2);
#endif
        }

    }

#if DEBUG
    for( int i = 0; i< v_greenContours.size(); i++ )
    {
        RNG rng(12345);
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        // contour
        drawContours( drawing, v_greenContours, i, Scalar(225,0,0), 1, 8, vector<Vec4i>(), 0, Point() );
        // ellipse
        ellipse( drawing, v_greenMinEllipse[i], Scalar(0,255,0), 2, 8 );
        // rotated rectangle
        Point2f rect_points[4]; v_greenMinRect[i].points( rect_points );
        for( int j = 0; j < 4; j++ )
            line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
    }
#endif

    vector<Point2f> v_center;
    for(int i=0;i<v_lightBar.size();i++)
    {
        for(int j=i+1;j<v_lightBar.size();j++)
        {
            for(int l=0;l<v_greenMinEllipse.size();l++)
            {
                double d_centerOfLightx=(v_lightBar[i].center.x+v_lightBar[j].center.x)*0.5;
                double d_centerOfLighty=(v_lightBar[i].center.y+v_lightBar[j].center.y)*0.5;
                d_dDistanceBetweenLightBarAndGreenRect=v_greenMinEllipse[l].size.area()*0.03;
                if(distance(d_centerOfLightx,d_centerOfLighty,v_greenMinEllipse[l].center.x,v_greenMinEllipse[l].center.y)<d_dDistanceBetweenLightBarAndGreenRect)
                {
                    v_center.push_back(Point2f(d_centerOfLightx,d_centerOfLighty));
                }
            }
        }
    }

    for(int i=0;i<v_center.size();i++)
    {
        circle(m_sourceImage,v_center[i],10,Scalar(0,255,0),3);
    }
    imshow("Final",m_sourceImage);



    //对所有灯条边缘做最小矩形


#if DEBUG
    imshow("contours",drawing);
    imshow("d_panel",m_dvalue);
    imshow("Canny",m_canny);
#endif

    return v_center;

}


Point2f getArmor (Mat m_sourceImage,int i_color )
{
    Mat m_pointMap=Mat::zeros( Size(650,490), CV_8U );
    if (v_pointSet.size()<6)
    {
        v_pointSet.push_back(findArmor(m_sourceImage,i_color));
        return Point2f(-1,-1);
    }
    else
    {
        v_pointSet.push_back(findArmor(m_sourceImage,i_color));
        v_pointSet.erase(v_pointSet.begin());
        for(int i=0;i<v_pointSet.size();i++)
        {
            for(int j=0;j<v_pointSet[i].size();j++){
                float x = v_pointSet[i][j].x;
                float y = v_pointSet[i][j].y;
                Mat m_temp=Mat::zeros( Size(650,490), CV_8U );
                // m_pointMap.at<uchar>(v_pointSet[i][j].y , v_pointSet[i][j].x ) += 255;
                circle(m_temp,v_pointSet[i][j],15,Scalar(255),-1);
                addWeighted(m_pointMap,1,m_temp,0.2,0,m_pointMap);



            }

        }
        //  threshold(m_pointMap,m_pointMap,160,255,0);
    }
    imshow("PointSet",m_pointMap);
}
