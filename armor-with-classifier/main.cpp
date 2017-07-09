#include <iostream>
#include "armor_sample_marker.h"

int main() {
    VideoCapture camera(0);
    camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    if(!camera.isOpened()) {
        return -1;
    }
    cvNamedWindow("src",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("color_panel",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("d_panel",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Canny",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("contours",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Final",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("green",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("greenCanny",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("PointSet",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("ROI",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("drawingROI",CV_WINDOW_AUTOSIZE);
    Mat frame;
    bool stop=false;


    while (!stop){
        camera>>frame;
        imshow("src",frame);
       // clock_t time1 = clock();
        findArmor(frame,ARMOR_BLUE);
        //clock_t time2 = clock();
       // cout<<time2-time1<<endl;
        cvWaitKey(1);

    }

}