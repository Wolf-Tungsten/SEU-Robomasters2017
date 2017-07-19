//
// Created by 狼剩子 on 2017/6/27.
//

#include "armor.h"
#define DEBUG 1




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
    imshow("And", m_and);
}

vector<Point2f> findArmor_d(Mat m_sourceImage ,int i_color)
{

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
