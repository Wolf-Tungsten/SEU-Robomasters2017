//
// Created by 狼剩子 on 2017/7/7.
//

#ifndef ARMOR_WITH_CLASSIFIER_ARMOR_SAMPLE_MARKER_H
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
#include <queue>

using namespace std;
using namespace cv;



const int ARMOR_BLUE = 0;
const int ARMOR_RED = 1;

vector<Point2f> findArmor(Mat m_sourceImage,int i_color );
Point getArmor (Mat m_sourceImage,int i_color );
#define ARMOR_ARMOR_H


#define ARMOR_WITH_CLASSIFIER_ARMOR_SAMPLE_MARKER_H

#endif //ARMOR_WITH_CLASSIFIER_ARMOR_SAMPLE_MARKER_H
