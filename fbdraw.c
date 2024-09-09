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
    ubyte *pfb, a = 0xFF;
    int color = vinfo.bits_per_pixel/8.;


    if(end_x == 0) end_x = vinfo.xres;
    if(end_y == 0) end_y = vinfo.yres;

    pfb = (ubyte *)mmap(0, vinfo.xres * vinfo.yres * color, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    for(int x = start_x; x < end_x*color; x+=color) {
        for(int y = start_y; y<end_y; y++) {
            *(pfb + (x+0) + y*vinfo.xres*color) = b;
            *(pfb + (x+1) + y*vinfo.xres*color) = g;
            *(pfb + (x+2) + y*vinfo.xres*color) = r;
            *(pfb + (x+3) + y*vinfo.xres*color) = a;
        }
    }

    munmap(pfb, vinfo.xres * vinfo.yres * color);
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
    
//    drawface(fbfd, 0, 0, 0, 0, 128, 100, 65);
    drawfacemmap(fbfd, 0, 0, 0, 0, 255, 100, 23);
    close(fbfd);

    return 0;
}
