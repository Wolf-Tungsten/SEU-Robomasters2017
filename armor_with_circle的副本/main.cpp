
#include "armor.h"
#include <ctime>
void   Delay(int   time)//time*1000为秒数
{
    clock_t  now   = clock();
    while(  clock()   -   now   <   time   );
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
    cvNamedWindow("color_panel",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("d_panel",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Canny",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("contours",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Final",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("green",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("greenCanny",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("PointSet",CV_WINDOW_AUTOSIZE);


    //Mat src = imread("/Users/WolfTungsten/Development/armor/testImage/8.jpg",CV_LOAD_IMAGE_COLOR);
    //findArmor(src,ARMOR_RED);
    //cvWaitKey(0);
    Mat frame;
    bool stop=false;
    Mat ROI = imread("/Users/WolfTungsten/Development/armor/output_small.jpg",CV_LOAD_IMAGE_COLOR);

    while (!stop){
        cap>>frame;
        //imshow("canny",frame);
        clock_t time1 = clock();
        getArmor(frame,ARMOR_RED);
        clock_t time2 = clock();
        cout<<time2-time1<<endl;
        cvWaitKey(1);

    }

    return 0;



}