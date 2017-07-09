
//
// Created by 狼剩子 on 2017/6/27.
//

#include "armor_sample_marker.h"
#define DEBUG 0

int i_cannyHi = 15; //Canny算子上阈值
int i_cannyLo = 45; //Canny算子下阈值
int i_highLight = 215; //灯条原图高阈值
int i_lowLight = 10; //灯条原图低阈值
double k = 2;//椭圆比例
double d_areaDivDistanceLo = 0.01;
double d_areaDivDistanceHi = 0.5;
int dilation_size =3;
int morpho_size = 3;
int border_size = 5;
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
        addWeighted(panel[0],1.5,panel[1],-0.75,0,m_dvalue);
        addWeighted(m_dvalue,1,panel[2],-0.75,0,m_dvalue);
        i_highLight = 250;
        i_lowLight =5;
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
        v_minRect[i]=minAreaRect(hull[i]); //灯条凸包
        double d_dpanelMin,d_dpanelMax,d_srcPanelMin,d_srcPanelMax;//灯条识别区域的最大值和最小值
        Point2f p_pointOfMinRect[4]; //获取灯条四个点
        v_minRect[i].points(p_pointOfMinRect);
        int i_minX = p_pointOfMinRect[0].x;
        int i_minY = p_pointOfMinRect[0].y;
        int i_maxX = p_pointOfMinRect[3].x;
        int i_maxY = p_pointOfMinRect[3].y;

        for(int j=0;j<4;j++)
        {
            if(i_minX>p_pointOfMinRect[j].x)
                i_minX=p_pointOfMinRect[j].x;
            if(i_minY>p_pointOfMinRect[j].y)
                i_minY=p_pointOfMinRect[j].y;
            if(i_maxX<p_pointOfMinRect[j].x)
                i_maxX=p_pointOfMinRect[j].x;
            if(i_maxY<p_pointOfMinRect[j].y)
                i_maxY=p_pointOfMinRect[j].y;
        }
        i_minX = (i_minX>=0)?(i_minX):0;
        i_minY = (i_minY>=0)?(i_minY):0;
        i_maxX = (i_maxX<640)?(i_maxX):639;
        i_maxY = (i_maxY<480)?(i_maxY):479;


        Mat m_dpanelROI,m_srcPanelROI;
        m_dpanelROI = m_dvalue(Rect(i_minX,i_minY,i_maxX-i_minX,i_maxY-i_minY));
        m_srcPanelROI = m_sourcePanel(Rect(i_minX,i_minY,i_maxX-i_minX,i_maxY-i_minY));
        //将灯条定义成感兴趣区域
        minMaxLoc(m_dpanelROI,&d_dpanelMin,&d_dpanelMax);
        minMaxLoc(m_srcPanelROI,&d_srcPanelMin,&d_srcPanelMax);
        //从灯条的感兴趣区域内读取最大值、最小值作为判断依据


        if(d_dpanelMin<i_lowLight && d_srcPanelMax>i_highLight) {//通过减色通道和原通道判断是否是灯条

            v_lightBar.push_back(v_minRect[i]);
#if DEBUG
            cout<<"减色："<<d_dpanelMin<<" 原通道："<<d_srcPanelMax<<endl;
            Point2f Drect[4];
            v_minRect[i].points(Drect);
            for (int j = 0; j < 4; j++)
                line(drawing, Drect[j], Drect[(j + 1) % 4], Scalar(0, 255, 255), 2);
#endif
        }

    }



    vector<Point2f> v_center;
//遍历所有灯条
    for(int i=0;i<v_lightBar.size();i++)
    {
        for(int j=i+1;j<v_lightBar.size();j++)
        {
            //任取两个灯条
            double d_lightBarArea =(v_lightBar[i].size.area()>v_lightBar[j].size.area())?v_lightBar[i].size.area():v_lightBar[i].size.area();
            double d_lightBarDistance = distance(v_lightBar[i].center.x,v_lightBar[i].center.y,
                                                 v_lightBar[j].center.x,v_lightBar[j].center.y);
            //cout<<"面积距离比："<<d_lightBarArea/d_lightBarDistance<<endl;
            //判断其面积和距离的比值
            if(d_lightBarArea/d_lightBarDistance>d_areaDivDistanceLo && d_lightBarArea/d_lightBarDistance<d_areaDivDistanceHi){

                Point2f p_point1[4];
                Point2f p_point2[4];
                Point2f p_point[8];
                v_lightBar[i].points(p_point1);
                v_lightBar[j].points(p_point2);
                for(int a = 0;a<8;a++)
                    p_point[a] = (a<4)?p_point1[a]:p_point2[a-4];
                int i_minX = p_point[0].x;
                int i_minY = p_point[0].y;
                int i_maxX = p_point[3].x;
                int i_maxY = p_point[3].y;

                for(int b=0;b<8;b++)
                {
                    if(i_minX>p_point[b].x)
                        i_minX=p_point[b].x-20;
                    if(i_minY>p_point[b].y)
                        i_minY=p_point[b].y-20;
                    if(i_maxX<p_point[b].x)
                        i_maxX=p_point[b].x+20;
                    if(i_maxY<p_point[b].y)
                        i_maxY=p_point[b].y+20;
                }

                int i_minOutputX = i_minX-border_size;
                int i_maxOutputX = i_maxX+border_size;
                int i_minOutputY = i_minY-border_size;
                int i_maxOutputY = i_maxY+border_size;


                i_minX = (i_minX>=0)?(i_minX):0;
                i_minY = (i_minY>=0)?(i_minY):0;
                i_maxX = (i_maxX<640)?(i_maxX):639;
                i_maxY = (i_maxY<480)?(i_maxY):479;

                Mat greenROI = green_canny(Rect(i_minX,i_minY,i_maxX-i_minX,i_maxY-i_minY));


                //将符合条件的灯条所在区域设置为感兴趣区域，并在绿色其中寻找轮廓
                findContours( greenROI, v_greenContours, v_greenHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
                vector<RotatedRect> v_greenMinRect(v_greenContours.size() );
                vector<RotatedRect> v_greenMinEllipse( v_greenContours.size() );

                for( int c = 0; c < v_greenContours.size(); c++ )
                    //从找出的绿色轮廓中拟合椭圆轮廓，判断椭圆离心率
                { v_greenMinRect[c] = minAreaRect( Mat(v_greenContours[c]) );
                    if(v_greenContours[c].size() > 30 &&
                       (v_greenMinRect[c].size.width>v_greenMinRect[c].size.height?(v_greenMinRect[c].size.width*1.0/v_greenMinRect[c].size.height):(v_greenMinRect[c].size.height*1.0/v_greenMinRect[c].size.width))<=k)
                    {
                        //cout<<"椭圆斜率"<<((v_greenMinRect[i].size.width>v_greenMinRect[i].size.height)?(v_greenMinRect[i].size.width*1.0/v_greenMinRect[i].size.height):(v_greenMinRect[i].size.height*1.0/v_greenMinRect[i].size.width))<<endl;
                        v_greenMinEllipse[c] = fitEllipse( Mat(v_greenContours[c]) );
                    }
                }
                //判断椭圆的圆心和两个灯条中心的距离是否符合要求

                for(int d=0;d<v_greenMinEllipse.size();d++)
                {
                    double d_centerOfLightx=(v_lightBar[i].center.x+v_lightBar[j].center.x)*0.5;
                    double d_centerOfLighty=(v_lightBar[i].center.y+v_lightBar[j].center.y)*0.5;
                    double d_dDistanceBetweenLightBarAndGreenRect=v_greenMinEllipse[d].size.area()*0.03;
                    if(distance(d_centerOfLightx,d_centerOfLighty,v_greenMinEllipse[d].center.x+i_minX,v_greenMinEllipse[d].center.y+i_minY)<d_dDistanceBetweenLightBarAndGreenRect)
                    {
                        v_center.push_back(Point2f(d_centerOfLightx,d_centerOfLighty));
                        marker(m_sourceImage, 100,i_minX,i_minY,i_maxX,i_maxY);
                        cout<<"发现装甲板["<<d_centerOfLightx<<","<<d_centerOfLighty<<"]"<<endl;
                    }
                }
                //v_center.push_back(Point2f(0.5*(v_lightBar[i].center.x+v_lightBar[j].center.x),0.5*(v_lightBar[i].center.y+v_lightBar[j].center.y)));
#if DEBUG
                for( int e = 0; e< v_greenContours.size(); e++ )
                {
                    RNG rng(12345);
                    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                    // contour
                    drawContours( drawing, v_greenContours, e, Scalar(225,0,0), 1, 8, vector<Vec4i>(), 0, Point() );
                    // ellipse
                    ellipse( drawing, v_greenMinEllipse[e], Scalar(0,255,0), 2, 8 );
                    // rotated rectangle
                    Point2f rect_points[4]; v_greenMinRect[e].points( rect_points );
                    for( int f = 0; f < 4; f++ )
                        line( drawing, rect_points[f], rect_points[(f+1)%4], color, 1, 8 );
                }
#endif
            }











            /*
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
             */
        }
    }






    for(int i=0;i<v_center.size();i++)
    {
        circle(m_sourceImage,v_center[i],10,Scalar(0,255,0),3);
    }
   // imshow("Final",m_sourceImage);



    //对所有灯条边缘做最小矩形


#if DEBUG
    imshow("contours",drawing);
    imshow("d_panel",m_dvalue);
    imshow("Canny",m_canny);
#endif

    return v_center;

}


Point getArmor (Mat m_sourceImage,int i_color )
{
    Mat m_pointMap=Mat::zeros( Size(650,490), CV_8U );
    if (v_pointSet.size()<4)
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
                circle(m_temp,v_pointSet[i][j],15,Scalar(25),-1);
                addWeighted(m_pointMap,1,m_temp,0.05*i*i,0,m_pointMap);



            }

        }
        //  threshold(m_pointMap,m_pointMap,160,255,0);
    }
    Point p_max;
    minMaxLoc(m_pointMap,NULL,NULL,NULL,&p_max);
    resize(m_sourceImage,m_sourceImage,Size(640,480));
    Scalar drawingColor(0,0,0);
    if(i_color == ARMOR_BLUE)
        drawingColor = Scalar(0,0,255);
    else
        drawingColor = Scalar(255,0,0);
    if(p_max.x!=0 && p_max.y!=0)
    circle(m_sourceImage,p_max,5,drawingColor,3);

    imshow("Final",m_sourceImage);
    imshow("PointSet",m_pointMap);
    return p_max;
}

void marker(Mat input,int size, int minX, int minY, int maxX, int maxY){
    int i_rectMinX = minX+5;
    int i_rectMinY = minY+10;
    int i_rectMaxX = maxX-5;
    int i_rectMaxY = maxY-10;
    minX -= border_size;
    minY -= border_size;
    maxX += border_size;
    maxY += border_size;
    static int filename=1;
    int i_areaSize;
    int i_expandSize;
    Mat m_src = input.clone();
    Mat m_drawing = input.clone();
    if((maxX-minX)>(maxY-minY)){
        i_areaSize = maxX-minX;
        i_expandSize = i_areaSize-(maxY-minY);
        minY -= 0.5 * i_expandSize;
    }
    else{
        i_areaSize = maxY-minY;
        i_expandSize = i_areaSize-(maxX-minX);
        minX -= 0.5 * i_expandSize;
    }
    try{


        Mat m_ROI = m_src(Rect(minX,minY,i_areaSize,i_areaSize)).clone();
        Mat m_drawingROI = m_drawing(Rect(minX,minY,i_areaSize,i_areaSize)).clone();

        resize(m_ROI, m_ROI, Size(size,size));
        imwrite("/Volumes/sd/SampleBlue1/"+to_string(filename++)+".jpg",m_ROI);
        imshow("ROI", m_ROI);

        return;

    }
    catch (...){
        cout<<"异常！"<<endl;
    }
}