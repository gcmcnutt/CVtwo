#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#include <iostream>
#include <fstream>
 
using namespace std;
using namespace cv;
 
int main(int argc, char** argv) {

    if (argc < 2) {
        cout << "Usage: ./horizon <input_video> <output_video>" << endl;
        return -1;
    }

    VideoCapture video(argv[1]);

    int num_frames = (int) video.get(CAP_PROP_FRAME_COUNT);
    int fps = (int) video.get(CAP_PROP_FPS);
    int width = (int) video.get(CAP_PROP_FRAME_WIDTH);
    int height = (int) video.get(CAP_PROP_FRAME_HEIGHT);

    printf("%d frames @ %d fps\n", num_frames, fps);

    Mat intrinsic = (Mat_<double>(3,3) <<
        299.39646639,    0.,          419.96165812,
           0.,          302.5602385,   230.25411049,
           0.,            0.,            1.
    );
    Mat distortion = (Mat_<double>(1,5) <<
        -0.16792771, 0.03121603, 0.00218195, -0.00026904, -0.00263317
    );

    VideoWriter writer(argv[2], VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, Size(width, height), true);
    std::ofstream horizonf(argv[2], std::ios::binary);

    Mat mask = imread("hb1-mask.jpg", IMREAD_GRAYSCALE);

    for (int f = 0; f < num_frames; f++) {
        // read next frame
        Mat inimg, outimg, resimg, maskimg, edges, nonzeros;
        bool success = video.read(inimg);
        if (!success) {
            std::cout << "Error on frame " << f << std::endl;
            break;
        }
        imshow("inimg", inimg);

        // undo camera distortion
        undistort(inimg, outimg, intrinsic, distortion);
        imshow("outimg", outimg);

        // resize to 1/4 size
        resize(outimg, resimg, Size2d(width / 4, height / 4), 0, 0, INTER_NEAREST);
        imshow("resimg", resimg);

        // edge detection
        Canny(resimg, edges, 200, 600, 3);
        imshow("edges", edges);

        // mask out aircraft
        edges.copyTo(maskimg, mask);
        imshow("maskimg", maskimg);

        // find horizon line
        Vec4f line;
        findNonZero(edges, nonzeros);
        if (nonzeros.total() != 0) {
            fitLine(nonzeros, line, 2, 0, 0.01, 0.01);  // 2 = CV_DIST_L2

            // find line properties
            if (line[0] != 0) {
                float slope = line[1] / line[0];
                float intercept = line[3] - slope * line[2];
                float x1 = -intercept / slope;
                float x2 = (outimg.rows - intercept) / slope;
                line[0] = x1;
                line[1] = 0;
                line[2] = x2;
                line[3] = outimg.rows;
            } else {
                line[0] = line[2];
                line[1] = 0;
                line[2] = line[0];
                line[3] = outimg.rows;
            }

            cv::line(outimg,
                Point2f(line[0], line[1]), Point2f(line[2], line[3]),
                CV_RGB(0, 0, 255),
                1,
                LINE_AA);
        }
        imshow("outimg", outimg);
        writer.write(outimg);

        if (f % 2400 == 0) {
            std::cout << 100.0 * (f / float(num_frames)) << std::endl;
        }

        // Press ESC on keyboard to exit
        char c = (char) waitKey(1000/fps);
        if (c==27)
            break;
    }
}