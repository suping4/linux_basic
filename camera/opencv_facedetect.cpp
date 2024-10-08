#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define FBDEVICE "/dev/fb0"
#define CAMERA_COUNT 3
#define CAM_WIDTH 640
#define CAM_HEIGHT 480

using namespace cv;

const static char *cascade_name = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml";

int main(int argc, char **argv){
    int fbfd;
    struct fb_var_screeninfo vinfo;
    unsigned char *buffer;
    unsigned short *pfbmap;
    unsigned int x, y, i, screensize, j;
    VideoCapture vc(0);
    CascadeClassifier cascade;
    Mat frame(CAM_WIDTH, CAM_HEIGHT, CV_8UC3, Scalar(255));
    Point pt1, pt2;

    if(!cascade.load(cascade_name)){
        perror("OpenCV : load cascade\n");
        return EXIT_FAILURE;
    }

    vc.set(CAP_PROP_FRAME_WIDTH, CAM_WIDTH);
    vc.set(CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);

    fbfd = open(FBDEVICE, O_RDWR);
    if(fbfd < 0){
        perror("OpenCV : open FrameBuffer\n");
        return EXIT_FAILURE;
    }

    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1){
        perror("OpenCV : ioctl FBIOGET_VSCREENINFO\n");
        return EXIT_FAILURE;
    }

    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8.;
    pfbmap = (unsigned short *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if(pfbmap == (unsigned short *)-1){
        perror("OpenCV : mmap\n");
        return EXIT_FAILURE;
    }
    memset(pfbmap, 0, screensize);

    for(i =0; i<CAMERA_COUNT; i++){
        int colors = vinfo.bits_per_pixel/8;
        long location=0;

        vc >> frame;
        Mat image(CAM_WIDTH, CAM_HEIGHT, CV_8UC1, Scalar(255));
        cvtColor(frame, image, COLOR_BGR2GRAY);
        equalizeHist(image, image);

        std::vector<Rect> faces;
        cascade.detectMultiScale(image, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

        for(j = 0; j < faces.size(); j++){
            pt1.x = faces[j].x;
            pt2.x = faces[j].x + faces[j].width;
            pt1.y = faces[j].y;
            pt2.y = faces[j].y + faces[j].height;
            rectangle(frame, pt1, pt2, Scalar(255, 0, 0), 3, 8);
        }

        buffer = (uchar *)frame.data;
        for(y = 0, location=0; y < frame.rows; y++){
            for(x=0; x<vinfo.xres; x++){
                if(x>=frame.cols){
                    location++;
                    continue;
                }
                unsigned char b = *(buffer+(y*frame.cols+x)*3+0);
                unsigned char g = *(buffer+(y*frame.cols+x)*3+1);
                unsigned char r = *(buffer+(y*frame.cols+x)*3+2);
                unsigned short pixel = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
                pfbmap[location++] = pixel;
            }
    
       }
    }

    Mat saveImage = Mat(frame.size(), CV_8UC3, buffer);
    imwrite("capture.bmp", saveImage);

    munmap(pfbmap, screensize);
    close(fbfd);
    return 0;
}
