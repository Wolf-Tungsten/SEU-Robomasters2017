
#include "armor.h"
#include <ctime>
void   Delay(int   time)//time*1000为秒数
{
    clock_t   now   =   clock();
    while(   clock()   -   now   <   time   );
}

int main( int argc, char** argv )
{
    VideoCapture cap(1);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    //cap.set(CV_CAP_PROP_AUTO_EXPOSURE, 0);

    cvNamedWindow("And", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("lightbarcontours", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("result",CV_WINDOW_AUTOSIZE);
    if(!cap.isOpened())
    {
        return -1;
    }

    Mat frame;
    bool stop=false;
    vector<Point2f> target_temp,target;

    while (!stop){
        int time1 = clock();
        //cap.set(CV_CAP_PROP_EXPOSURE, -10);
        cap>>frame;
        Mat temp = frame.clone();
        target_temp = findArmor(frame, ARMOR_RED);

            target = target_temp;

        for (int i=0;i<target.size();i++)
        {
            circle(temp, target[i],5,Scalar(0,255,255),3);

        }
        imshow("result", temp);
        int time2 = clock();
        cout<<"耗时:"<<(time2-time1)/1000.0<<"ms"<<endl;
        cvWaitKey(1);
    }




}