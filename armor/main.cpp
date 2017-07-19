
#include "armor.h"
#include <ctime>
void   Delay(int   time)//time*1000为秒数
{
    clock_t   now   =   clock();

    while(   clock()   -   now   <   time   );
}

int main( int argc, char** argv )
{
    VideoCapture cap(0);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    //cap.set(CV_CAP_PROP_AUTO_EXPOSURE, 0);

    cvNamedWindow("And", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("HSV mask", CV_WINDOW_AUTOSIZE);
    if(!cap.isOpened())
    {
        return -1;
    }

    Mat frame;
    bool stop=false;
    while (!stop){
        cap.set(CV_CAP_PROP_EXPOSURE, -10);
        cap>>frame;
        findArmor(frame, ARMOR_RED);
        cvWaitKey(10);
    }




}