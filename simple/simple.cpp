#pragma comment(lib,"winmm.lib")
#include <iostream>
#include <stdio.h>
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include"opencv2/flann.hpp"
#include"opencv2/xfeatures2d.hpp"
#include"opencv2/ml.hpp"
#include <Windows.h>
using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
using namespace cv::ml;
const string videofile = "F:\\Lab\\data\\s1.mp4";
void getH(Mat&H, Mat&a, Mat&b);
void find_rect(Mat &findc,Mat &src);
int main()
{
    cout<<"hi"<<endl;
    VideoCapture cap(videofile);
    if(!cap.isOpened()){
        cout<<"error"<<endl;
        return 0;
    }
    int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    VideoWriter video("cvSub1.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height));
    cout<<"hi"<<endl;
    Mat frame, diff_gray, background_frame, diff_frame,bin_diff_frame,last_frame,background_gray,last_gray,H,background_frame_h,background_gray_h;
    cap.read(background_frame);
    //video.write(background_frame);
    while(waitKey(1)!=27 && cap.read(frame)){
        // absdiff(background_frame, frame, diff_frame);
        // erode(diff_frame, diff_frame, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3)));
        // threshold(diff_frame, diff_frame, 50, 255, cv::THRESH_BINARY);
        // imshow("bin_diff_frame",diff_frame);
        // video.write(diff_frame);
        frame.copyTo(last_frame);
    }
    //imshow("background_frame",background_frame);
    //imshow("last_frame",last_frame);
    cap.release();
    video.release();


    cvtColor(background_frame,background_gray,COLOR_BGR2GRAY);
    cvtColor(last_frame,last_gray,COLOR_BGR2GRAY);
    getH(H,background_gray,last_gray);
    //warpPerspective(background_gray, background_gray_h, H, background_gray.size());
    warpPerspective(background_frame, background_frame_h, H, background_frame.size());
    //imshow("last_gray",last_gray);
    //imshow("background_gray",background_gray);
    //imshow("background_gray_h",background_gray_h);
    //imshow("background_frame_h",background_frame_h);
    absdiff(background_frame_h, last_frame, diff_frame);
    erode(diff_frame, diff_frame, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5)));
    threshold(diff_frame, diff_frame, 100, 255, cv::THRESH_BINARY);
    cvtColor(diff_frame,diff_gray,COLOR_BGR2GRAY);
    erode(diff_gray, diff_gray, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7,7)));    
    find_rect(diff_gray,last_frame);
    imshow("diff_gray",diff_gray);
    imshow("last_frame",last_frame);
    imwrite("last_fram.jpg",last_frame);
    
    // absdiff(background_frame, last_frame, diff_frame);
    // erode(diff_frame, diff_frame, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3)));
    // threshold(diff_frame, diff_frame, 50, 255, cv::THRESH_BINARY);
    // imshow("bin_diff_frame2",diff_frame);

    waitKey(0);
    return 0;
}

void getH(Mat&H, Mat&a, Mat&b)
{
	//Ptr<ORB> surf;
    Ptr<SURF> surf;      //创建方式和2中的不一样
	//create(double hessianThreshold = 100,int nOctaves=4,int nOctaveLayers=3,bool extended = false,bool upright=false) 	
    surf = SURF::create(800);
    //surf = ORB::create();
	BFMatcher matcher;
    Mat c, d;
    vector<KeyPoint>key1, key2;
    vector<DMatch> matches;

    surf->detectAndCompute(a, Mat(), key1, c);
    surf->detectAndCompute(b, Mat(), key2, d);

    matcher.match(c, d, matches);       //匹配

    sort(matches.begin(), matches.end());  //筛选匹配点
    vector< DMatch > good_matches;
    int ptsPairs = std::min(50, (int)(matches.size() * 0.15));
    cout << ptsPairs << endl;
    for (int i = 0; i < ptsPairs; i++)
    {
        good_matches.push_back(matches[i]);
    }
    Mat outimg;
    drawMatches(a, key1, b, key2, good_matches, outimg, Scalar::all(-1), Scalar::all(-1),vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点

    std::vector<Point2f> obj;
    std::vector<Point2f> scene;

    for (size_t i = 0; i < good_matches.size(); i++)
    {
        obj.push_back(key1[good_matches[i].queryIdx].pt);
        scene.push_back(key2[good_matches[i].trainIdx].pt);
    }

    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = Point(0, 0);
    obj_corners[1] = Point(a.cols, 0);
    obj_corners[2] = Point(a.cols, a.rows);
    obj_corners[3] = Point(0, a.rows);
    std::vector<Point2f> scene_corners(4);

    H = findHomography(obj, scene, RANSAC);      //寻找匹配的图像
    cout<<endl<<H<<endl;

}

void find_rect(Mat &findc,Mat &src)
{
    vector<vector<Point>> contours;
    cv::findContours(findc, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    const int maxArea = 1000;
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