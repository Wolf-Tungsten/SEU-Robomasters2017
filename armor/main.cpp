
#include "armor.h"
#include <ctime>
void   Delay(int   time)//time*1000为秒数
{
    clock_t   now   =   clock();

    while(   clock()   -   now   <   time   );
}
extern int i_greenCannyHi;
extern int i_greenCannyLo;
void on_trackbar(int,void*)
{return;}
int main( int argc, char** argv )
{
    VideoCapture cap(1);
    if(!cap.isOpened())
    {
        return -1;
    }
    cvNamedWindow("color_pannel",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("d_pannel",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Canny",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("contours",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("final",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("green",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("greenRect",CV_WINDOW_AUTOSIZE);
    createTrackbar("Canny Hi","contours",&i_greenCannyHi,255,on_trackbar);
    createTrackbar("Canny Lo","contours",&i_greenCannyLo,255,on_trackbar);

    Mat src = imread("/Users/WolfTungsten/Development/armor/testImage/8.jpg",CV_LOAD_IMAGE_COLOR);
    findArmor(src,ARMOR_RED);
    cvWaitKey(0);
    Mat frame;
    bool stop=false;
    while (!stop){
        cap>>frame;
        findArmor(frame,ARMOR_BLUE);
        cvWaitKey(10);
    }

    return 0;



}