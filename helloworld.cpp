#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    VideoCapture cap = VideoCapture(0);
    Mat frame;
    cap >> frame;
    imshow("Display Window", frame);
    waitKey(0);
    return 0;
}