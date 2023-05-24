#include "opencv2/opencv.hpp"
#include <iostream>
 
using namespace std;
using namespace cv;
 
int main(int argc, char** argv) {
  // Create a VideoCapture object and open the input file
  // If the input is the web camera, pass 0 instead of the video file name
  VideoCapture cap(argv[1]); 
    
  // Check if camera opened successfully
  if (!cap.isOpened()) {
    cout << "Error opening video stream or file" << endl;
    return -1;
  }
   
  int fps = (int) cap.get(CAP_PROP_FPS);
  printf("* FPS: %d\n", fps);

  while (1) {
 
    Mat frame;
    // Capture frame-by-frame
    cap >> frame;
  
    // If the frame is empty, break immediately
    if (frame.empty()) {
      break;
    }
 
    // Display the resulting frame
    imshow(argv[1], frame);
 
    // Press ESC on keyboard to exit
    char c = (char) waitKey(1000/fps);
    if (c==27)
      break;
  }
  
  // When everything done, release the video capture object
  cap.release();
 
  // Closes all the frames
  destroyAllWindows();
   
  return 0;
}