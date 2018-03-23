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

int main(int argc, char *argv[])
{
    Mat backGround, binary_result;
    VideoCapture cap("staticCam.mp4");//打开默认的摄像头
    if (!cap.isOpened())
    {
        return -1;
    }
    Mat frame;
    cap >> backGround;
    IplImage* g_pGrayImage, *g_pBinaryImage;
    IplImage iplBackGround = IplImage(backGround);
    cvShowImage("backGround",&iplBackGround);
    IplImage* dst = cvCreateImage(cvGetSize(&iplBackGround),iplBackGround.depth,iplBackGround.nChannels);
    int i=0;
    while (waitKey(80)!=27)
    {
        cap >> frame;
		if(frame.empty())
			break;
        IplImage iplFrame = IplImage(frame);
        cvSub(&iplBackGround,&iplFrame,dst);
        cvShowImage("frame",&iplFrame);
        cvShowImage("dst",dst);
        cout<<i++<<endl;
        g_pGrayImage =  cvCreateImage(cvGetSize(&iplFrame), IPL_DEPTH_8U, 1);
        cvCvtColor(&iplFrame, g_pGrayImage, CV_BGR2GRAY);
        // 创建二值图  
        g_pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);  
        cvThreshold(g_pGrayImage, g_pBinaryImage, 30, 255, CV_THRESH_BINARY);   
        cvShowImage("g_pGrayImage",g_pGrayImage);
    }
    cout<<"return"<<endl;
    return 0;
}
