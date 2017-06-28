//
// Created by 狼剩子 on 2017/6/27.
//

#include "armor.h"
#define DEBUG 1

int i_cannyHi = 25; //Canny算子上阈值
int i_cannyLo = 80; //Canny算子下阈值
int i_highLight = 215; //灯条原图高阈值
int i_lowLight = 10; //灯条原图低阈值
double k = 1;//灯条比例
double d_dAngle = 5; //灯条斜率差距的阈值
double d_minDistance = 300; //灯条中心距离的阈值
double d_greenRectArea = 2000; //绿通道装甲板面积
double d_dDistanceBetweenLightBarAndGreenRect =40; //灯条和边距间的垂直距离
double d_dAngleBetweenLightBarAndGreenRect = 180;
int i_greenCannyHi = 15;
int i_greenCannyLo = 75;


double distance(double x1,double y1,double x2,double y2){
return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}




vector<int> findArmor(Mat m_sourceImage,int i_color )
{
    resize(m_sourceImage,m_sourceImage,Size(640,480));
    //m_sourceImage.convertTo(m_sourceImage,m_sourceImage.type(),3,-400);

    Mat m_sourcePlane,m_dvalue; //分别存储指定图像对应颜色通道的数组、指定颜色通道做差后的数组
    vector<Mat> panel;
    split(m_sourceImage,panel);
    if(i_color == ARMOR_BLUE)
    {
        m_sourcePlane = panel[0].clone();
        addWeighted(panel[0],1,panel[1],-0.5,0,m_dvalue);
        addWeighted(m_dvalue,1,panel[2],-0.5,0,m_dvalue);
    }
    else if (i_color == ARMOR_RED)
    {
        m_sourcePlane = panel[2].clone();
        addWeighted(panel[2],1,panel[0],-0.5,0,m_dvalue);
        addWeighted(m_dvalue,1,panel[1],-0.5,0,m_dvalue);
    }
    else {
        cerr << "Color set Error!" << endl;
        vector<int> error;
        error.push_back(-1);
        error.push_back(-1);
        return error;
    }

    //查找矩形的部分
    Mat m_green ;
    //addWeighted(panel[1],1,panel[0],-1,0,m_green);
    m_green = m_sourceImage.clone();
    Mat m_greenCanny;
    //对绿色通道求算Canny算子
    Canny(m_green,m_greenCanny,15,75,3);

    //查找绿色通道的边缘
    vector<vector<Point> > v_greenContours;
    vector<Vec4i> v_greenHierarchy;
    GaussianBlur(m_greenCanny,m_greenCanny,Size(3,3),2);
    findContours( m_greenCanny, v_greenContours, v_greenHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_KCOS, Point(0, 0) );
    //绘制最小矩形
    vector<RotatedRect> v_minGreenRect( v_greenContours.size() );
    for( int i = 0; i < v_greenContours.size(); i++ )
    {
        v_minGreenRect[i] = minAreaRect( Mat(v_greenContours[i]) );
    }



    Mat m_canny;
    //高斯模糊

    GaussianBlur(m_dvalue,m_canny,Size(3,3),0);
    //Canny算子
    Canny(m_canny,m_canny,i_cannyLo,i_cannyHi);
    //寻找灯条边缘
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( m_canny, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    //对所有灯条边缘做最小矩形
    vector<RotatedRect> v_minRect( contours.size() );
    for( int i = 0; i < contours.size(); i++ ) {
        v_minRect[i] = minAreaRect( Mat(contours[i]) );
    }
    //通过亮度和长宽比例选出属于灯条的矩形
    vector<RotatedRect> v_lightBar_Rect;
    for( int i = 0; i< v_minRect.size(); i++ ) {
        Point p_centerOfRect = v_minRect[i].center;
        if(m_sourcePlane.at<uchar>(p_centerOfRect)>=i_highLight &&
           m_dvalue.at<uchar>(p_centerOfRect)<=i_lowLight)
        {
            //if(v_minRect[i].size.height/v_minRect[i].size.width>=k
            //&& abs(v_minRect[i].angle)<=20
            // ) {
            v_lightBar_Rect.push_back(v_minRect[i]);
            // }

        }
    }
    vector<RotatedRect> v_suitGreenRect;//从绿色通道中选取出面积合适的矩形框

    Mat m_greenDrawing = Mat::zeros( m_greenCanny.size(), CV_8UC3 );
    for( int i = 0; i< v_greenContours.size(); i++ ) {
        // rotated rectangle
        if (v_minGreenRect[i].size.area() > d_greenRectArea && v_minGreenRect[i].size.area() < 60000) {
            for (int j = 0; j < v_lightBar_Rect.size(); j++) {
                if(distance(v_minGreenRect[i].center.x,v_minGreenRect[i].center.y,v_lightBar_Rect[j].center.x,v_lightBar_Rect[j].center.y)<10000)

                v_suitGreenRect.push_back(v_minGreenRect[i]);
            }

        }
        }



#if DEBUG
    imshow("green",m_green);
    imshow("greenRec",m_greenDrawing);
    imshow("color_pannel",m_sourcePlane);
    //提高对比度
    equalizeHist(m_sourcePlane,m_sourcePlane);
    m_dvalue.convertTo(m_dvalue,m_dvalue.type(),2);
    //阈值化
    //threshold(m_dvalue,m_dvalue,80,255,0);
    imshow("d_pannel",m_dvalue);
#endif

#if DEBUG
    imshow("Canny",m_canny);
#endif





    vector<Point2f> v_center;
    for(int i=0;i<v_suitGreenRect.size();i++)
    {
                        v_center.push_back(v_suitGreenRect[i].center);
                    
        }




    //通过矩形的角度、长度关系判断两个灯条是否构成装甲板，并记录装甲板的中心点

    /*
    for(int i=0; i<v_lightBar_Rect.size();i++)
    {
        cout<<"矩形"<<i<<":{"<<endl;
        cout<<"height:"<<v_lightBar_Rect[i].size.height<<endl;
        cout<<"width:"<<v_lightBar_Rect[i].size.width<<endl;
        cout<<"angle:"<<v_lightBar_Rect[i].angle<<endl;
        cout<<"center:"<<v_lightBar_Rect[i].center<<endl;
    }
    for(int i=0;i<v_lightBar_Rect.size();i++) {
        for (int j = i + 1; j < v_lightBar_Rect.size(); j++) {
            //计算中心点点距
            double d_distance = (v_lightBar_Rect[i].center.x-v_lightBar_Rect[j].center.x)
            *(v_lightBar_Rect[i].center.x-v_lightBar_Rect[j].center.x)
            +(v_lightBar_Rect[i].center.y-v_lightBar_Rect[j].center.y)
            *(v_lightBar_Rect[i].center.y-v_lightBar_Rect[j].center.y);

            if (v_lightBar_Rect[i].angle * v_lightBar_Rect[j].angle >= 0 &&
                abs(v_lightBar_Rect[i].angle - v_lightBar_Rect[j].angle) <= d_dAngle &&
                d_distance>d_minDistance &&
                abs(v_lightBar_Rect[i].center.x-v_lightBar_Rect[j].center.x)/
                        ((v_lightBar_Rect[i].size.width+v_lightBar_Rect[j].size.width)/2)>=5
                    ) {


                float f_x = (v_lightBar_Rect[i].center.x + v_lightBar_Rect[j].center.x) / 2.0;
                float f_y = (v_lightBar_Rect[i].center.y + v_lightBar_Rect[j].center.y) / 2.0;

                if (f_x > 0 && f_y > 0) {
                    v_center.push_back(Point2f(f_x, f_y));
                }


            }

        }
    }
    */
#if DEBUG
    RNG rng(12345);
    /// 绘出轮廓
    Mat drawing = Mat::zeros( m_dvalue.size(), CV_8UC3 );
    for( int i = 0; i< v_lightBar_Rect.size(); i++ )
    {
        Scalar color = Scalar( 255, 255, 255);
        // rotated rectangle
        Point2f rect_points[4];
        v_lightBar_Rect[i].points( rect_points );
        for( int j = 0; j < 4; j++ )
            line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
    }
    for( int i = 0; i< v_suitGreenRect.size(); i++ )
    {
        Scalar color = Scalar( 0, 255, 0);
        // rotated rectangle
        Point2f rect_points[4];
        v_suitGreenRect[i].points( rect_points );
        for( int j = 0; j < 4; j++ )
            line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
    }
    for (int i = 0; i<v_center.size(); i++)
    {
        circle(drawing,v_center[i],10,Scalar(0,255,0),1);
        circle(m_sourceImage,v_center[i],10,Scalar(0,255,0),1);
        cout<<"发现装甲板["<<v_center[i].x<<","<<v_center[i].y<<"]"<<endl;
    }
    addWeighted(m_greenDrawing,1,drawing,1,0,drawing);
    imshow("contours",drawing);
    imshow("final",m_sourceImage);
#endif

    vector<int> result;
    result.push_back(-1);
    result.push_back(-1);
    return result;

}