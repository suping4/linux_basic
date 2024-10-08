#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

#define FBDEVICE "/dev/fb0"
#define CAMERA_COUNT 100
#define CAM_WIDTH 1280
#define CAM_HEIGHT 720

int main(int argc, char** argv){
    int fbfd;
    struct fb_var_screeninfo vinfo;

    unsigned char *buffer;
    unsigned short *pfbmap;
    unsigned int x, y, i, screensize;
    VideoCapture vc(0);
    Mat image(CAM_WIDTH, CAM_HEIGHT, CV_8UC3, Scalar(255));
    if(!vc.isOpened()){
        perror("OpenCV : open WebCam\n");
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

    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    pfbmap = (unsigned short *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if(pfbmap == nullptr){
        perror("OpenCV : mmap\n");
        return EXIT_FAILURE;
    }
    memset(pfbmap, 0, screensize);

    for(int i=0; i<CAMERA_COUNT; i++){
        int colors = vinfo.bits_per_pixel/8;
        long location = 0;
        int istride = CAM_WIDTH*2;

        vc >> image;
        buffer = (uchar*)image.data;
        for(y=0, location=0; y<image.rows; y++){
            for(x=0; x<vinfo.xres; x++){
                if(x >= image.cols) {
                    location++;
                    continue;
                }
                unsigned char r = *(buffer+(y*image.cols+x)*3+2);
                unsigned char g = *(buffer+(y*image.cols+x)*3+1);
                unsigned char b = *(buffer+(y*image.cols+x)*3+0);
                unsigned short pixel =((r>>3)<<11)|((g>>2)<<5)|(b>>3);                 /* 16비트 컬러로 전환 */
                pfbmap[location++] = pixel;
            }
        }
    }

    munmap(pfbmap, screensize);
    close(fbfd);

    return 0;
}
