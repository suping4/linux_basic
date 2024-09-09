#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define FBDEVICE "/dev/fb0"
#define COL 640

typedef unsigned char ubyte;

struct fb_var_screeninfo vinfo;

unsigned short makepixel(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned short)((r>>3)<<11 | ((g>>2)<<5) | (b>>3));
}

static void drawpoint(int fd, int x, int y, ubyte r, ubyte g, ubyte b) {
    ubyte a = 0xFF;

    unsigned short pixel;
    int offset = (x + y * vinfo.xres)* vinfo.bits_per_pixel/8.;
    lseek(fd, offset, SEEK_SET);
 
    pixel = makepixel(r, g, b);
    write(fd, &pixel, 2);
}


static void drawline(int fd, int start_x, int end_x, int y, ubyte r, ubyte g, ubyte b) {
    ubyte a = 0xFF;
    
    for(int x = start_x; x < end_x; x++) {

        int offset = (x + y * vinfo.xres)* vinfo.bits_per_pixel/8.;
        lseek(fd, offset, SEEK_SET);
        write(fd, &b,1);
        write(fd, &g,1);
        write(fd, &r,1);
        write(fd, &a,1);
    }
}


static void drawcircle(int fd, int center_x, int center_y, int radius, ubyte r, ubyte g, ubyte b) {
    int x = radius, y = 0;
    int radiusError = 1-x;

    while(x>=y){
        drawpoint(fd, x + center_x, y + center_y, r, g, b); 
        drawpoint(fd, y + center_x, x + center_y, r, g, b); 
        drawpoint(fd, -x + center_x, y + center_y, r, g, b); 
        drawpoint(fd, -y + center_x, x + center_y, r, g, b); 
        drawpoint(fd, -x + center_x, -y + center_y, r, g, b); 
        drawpoint(fd, -y + center_x, -x + center_y, r, g, b); 
        drawpoint(fd, x + center_x, -y + center_y, r, g, b); 
        drawpoint(fd, x + center_x, -y + center_y, r, g, b); 
        drawpoint(fd, y + center_x, -x + center_y, r, g, b); 

        y++;
        if (radiusError < 0) {
            radiusError += 2 * y + 1;
        } else{
             x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}
static void drawface(int fd, int start_x, int start_y, int end_x, int end_y, ubyte r, ubyte g, ubyte b) {
    ubyte a = 0xFF;

    if(end_x == 0) end_x = vinfo.xres;
    if(end_y == 0) end_y = vinfo.yres;

    for(int x = start_x; x < end_x; x++) {
        for(int y = start_y; y<end_y; y++) {
            int offset = (x + y * vinfo.xres)* vinfo.bits_per_pixel/8.;
            lseek(fd, offset, SEEK_SET);
            write(fd, &b, 1);
            write(fd, &g, 1);
            write(fd, &r, 1);
            write(fd, &a, 1);
        }
    }
}


static void drawfacemmap(int fd, int start_x, int start_y, int end_x, int end_y, ubyte r, ubyte g, ubyte b) {
    unsigned short *pfb; // 16비트 색상을 위해 unsigned short 사용
    int bytes_per_pixel = 2; // 16비트 색상은 2바이트

    // 화면 크기 설정
    if(end_x == 0) end_x = vinfo.xres;
    if(end_y == 0) end_y = vinfo.yres;

    // 메모리 매핑
    pfb = (unsigned short *)mmap(0, vinfo.xres * vinfo.yres * bytes_per_pixel, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    for(int y = start_y; y < end_y; y++) {
        for(int x = start_x; x < end_x; x++) {
            long location = (x + y * vinfo.xres);
            
            // 16비트 색상 생성 (5비트 빨강, 6비트 초록, 5비트 파랑)
            unsigned short color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
            
            // 프레임버퍼에 색상 쓰기
            *(pfb + location) = color;
        }
    }

    // 메모리 매핑 해제
    munmap(pfb, vinfo.xres * vinfo.yres * bytes_per_pixel);
}

int main(int argc, char **argv) {
    
    int fbfd, status, offset;


    unsigned short pixel;

    fbfd = open(FBDEVICE, O_RDWR);
    if(fbfd < 0) {
        perror("Error : cannot open framebuffer device");
        return -1;
    }

    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("Error reading ifxed informaion");
        return -1;
    }

    pixel = makepixel(255, 0, 0);
    write(fbfd, &pixel, 2);

    drawpoint(fbfd, 50, 50, 255, 0, 0);
    drawpoint(fbfd, 100, 100, 0, 255, 0);
    drawpoint(fbfd, 150, 150, 0, 0, 255);

    drawline(fbfd, 0, 100 , 200, 0, 255, 255);

    drawcircle(fbfd, 200, 200, 100 ,255, 0, 255);
    
// 프랑스 국기 색상
    ubyte blue_r = 0, blue_g = 0, blue_b = 255;   // 파란색
    ubyte white_r = 255, white_g = 255, white_b = 255; // 흰색
    ubyte red_r = 255, red_g = 0, red_b = 0;    // 빨간색

    // 각 색상 영역의 너비 계산 (1600을 3으로 나눔)
    int stripe_width = 1600 / 3;

    // 파란색 영역 그리기
    drawfacemmap(fbfd, 0, 0, stripe_width, 900, blue_r, blue_g, blue_b);

    // 흰색 영역 그리기
    drawfacemmap(fbfd, stripe_width, 0, stripe_width * 2, 900, white_r, white_g, white_b);

    // 빨간색 영역 그리기
    drawfacemmap(fbfd, stripe_width * 2, 0, 1600, 900, red_r, red_g, red_b);
    close(fbfd);

    return 0;
}
