#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "highgui.lib")
#pragma comment(lib, "cxcore.lib")

int main(int argc, char** argv){
    IplImage *image1 = cvLoadImage("sample1.jpg", CV_LOAD_IMAGE_COLOR);
    IplImage *image2 = cvLoadImage("sample2.jpg", CV_LOAD_IMAGE_COLOR);
    IplImage *image_add = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 3);
    IplImage *image_sub = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 3);
    IplImage *image_mul = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 3);
    IplImage *image_div = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 3);
    IplImage *image_gray1 = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 1);
    IplImage *image_gray2 = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 1);
    IplImage *image_white = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 1);
    IplImage *image_gray_sub = cvCreateImage(cvGetSize(image1), IPL_DEPTH_8U, 1);

    cvNamedWindow("image1", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("image2", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("image_add", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("image_sub", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("image_mul", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("image_div", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("image_gray1", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("image_gray2", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("image_white", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("image_gray_sub", CV_WINDOW_AUTOSIZE);

    cvAdd(image1, image2, image_add, NULL);
    cvSub(image1, image2, image_sub, NULL);
    cvMul(image1, image2, image_mul, 1);
    cvDiv(image1, image2, image_div, 1);
    cvCvtColor(image1, image_gray1, CV_RGB2GRAY);
    cvCvtColor(image2, image_gray2, CV_RGB2GRAY);
    cvAbsDiff(image_gray1, image_gray2, image_gray_sub);
    cvThreshold(image_gray_sub, image_white, 100, 255, CV_THRESH_BINARY);

    cvShowImage("image1", image1);
    cvShowImage("image2", image2);
    cvShowImage("image_add", image_add);
    cvShowImage("image_sub", image_sub);
    cvShowImage("image_mul", image_mul);
    cvShowImage("image_div", image_div);
    cvShowImage("image_gray1", image_gray1);
    cvShowImage("image_gray2", image_gray2);
    cvShowImage("image_white", image_white);
    cvShowImage("image_gray_sub", image_gray_sub);

    cvWaitKey(0);

    cvReleaseImage(&image1);

    cvDestroyAllWindows();

    return 0;

}