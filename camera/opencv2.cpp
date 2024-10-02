#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char** argv){
    Mat image1 = imread("mandrill.jpg", IMREAD_COLOR);
    Mat image2 = imread("sample-1.jpg", IMREAD_COLOR);
    Mat image_add = Mat::zeros(image1.size(), image1.type());
    Mat image_sub = Mat::zeros(image1.size(), image1.type());
    Mat image_div = Mat::zeros(image1.size(), image1.type());
    Mat image_mul = Mat::zeros(image1.size(), image1.type());
    Mat image_gray1 = Mat::zeros(image1.size(), CV_8UC1);
    Mat image_gray2 = Mat::zeros(image1.size(), CV_8UC1);
    Mat image_gray_sub = Mat::zeros(image1.size(), CV_8UC1);
    Mat image_white = Mat::zeros(image1.size(), CV_8UC1);

    add(image1, image2, image_add);
    subtract(image1, image2, image_sub);
    divide(image1, image2, image_div);
    multiply(image1, image2, image_mul);
    cvtColor(image1, image_gray1, COLOR_BGR2GRAY);
    cvtColor(image2, image_gray2, COLOR_BGR2GRAY);
    absdiff(image_gray1, image_gray2, image_gray_sub);
    threshold(image_gray_sub, image_white, 100, 255, THRESH_BINARY);

    imshow("Original images1", image1);
    imshow("Original images2", image2);
    imshow("Add images", image_add);
    imshow("Sub images", image_sub);
    imshow("Div images", image_div);
    imshow("Mul images", image_mul);
    imshow("Gray1 images", image_gray1);
    imshow("Gray2 images", image_gray2);
    imshow("Gray_sub images", image_gray_sub);
    imshow("White images", image_white);

    waitKey(0);

    destroyAllWindows();

    return 0;

}