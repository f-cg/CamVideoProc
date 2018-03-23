#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
using namespace cv;
using namespace std;

const int Train = 100;
int main(int argc, char *argv[])
{
    Ptr<BackgroundSubtractorMOG2> mog = createBackgroundSubtractorMOG2(100, 16*4, false);
    //bgsubtractor->setVarThreshold(20);
    Mat foreGround;
    Mat backGround;
    int trainCounter = 0;

    namedWindow("src");
    namedWindow("foreground");

    VideoCapture cap("1.MP4");//打开默认的摄像头
    if (!cap.isOpened())
    {
        return -1;
    }
    Mat src;
    bool stop = false;
    while (!stop)
    {
        cap >> src;

        mog->apply(src, foreGround, 0.005);
        //图像处理过程
        //medianBlur(foreGround, foreGround, 3);
        //dilate(foreGround, foreGround, Mat(), Point(-1, -1), 3);
        //erode(foreGround, foreGround, Mat(), Point(-1, -1), 6);
        //dilate(foreGround, foreGround, Mat(), Point(-1, -1), 3);
        imshow("foreground", foreGround);
        if (trainCounter < Train)//训练期间所得结果为不准确结果，不应作为后续
        {
            Mat findc;
            foreGround.copyTo(findc);
            vector<vector<Point>> contours;
            cv::findContours(findc, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

            //targets.clear();
            const int maxArea = 800;
            size_t s = contours.size();
            for (size_t i = 0; i < s; i++)
            {
                double area = abs(contourArea(contours[i]));
                if (area > maxArea)
                {
                    Rect mr = boundingRect(Mat(contours[i]));
                    rectangle(src, mr, Scalar(0, 0, 255), 2, 8, 0);
                    //targets.push_back(mr);
                }
            }
            //string text;
            char text[50];
            sprintf(text, "background training -%d- ...", trainCounter);
            putText(src, text, Point(50, 50), 3, 1, Scalar(0, 255, 255), 2, 8, false);
            //delete[] text;

        } else
        {
            //detects.clear();
            Mat findc;
            foreGround.copyTo(findc);
            vector<vector<Point>> contours;
            cv::findContours(findc, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
            const int maxArea = 500;
            size_t s = contours.size();
            RNG rng;
            for (size_t i = 0; i < s; i++)
            {
                double area = abs(contourArea(contours[i]));
                if (area > maxArea)
                {
                    Scalar sca_color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
                    Rect mr = boundingRect(Mat(contours[i]));
                    rectangle(src, mr, sca_color, 2, 8, 0);

                    //可以对动态目标进行相应操作
                }
            }
        }
        trainCounter++;
        imshow("src", src);
        if (waitKey(30) == 27) //Esc键退出
        {
            stop = true;
        }
    }
    return 0;
}

