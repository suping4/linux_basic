#include <stdio.h>
#include "bmpHeader.h"

int readBmp(char *filename, unsigned char **data, int *cols, int *rows) {
    BITMAPFILEHEADER bmpHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    FILE *fp;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("ERROR\n");
        return -1;
    }

    // 파일 헤더 읽기
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    // 정보 헤더 읽기
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    if(bmpInfoHeader.biBitCount != 24) {
        printf("This image doesn't supports 24 bit color\n");
        fclose(fp);
        return -1;
    }

    *cols = bmpInfoHeader.biWidth;
    *rows = bmpInfoHeader.biHeight;

    printf("Resolution: %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit per pixel: %d\n", bmpInfoHeader.biBitCount);

    fseek(fp, bmpHeader.bfOffBits, SEEK_SET);
    fread(*data, 1, bmpHeader.bfSize - bmpHeader.bfOffBits, fp);

    fclose(fp);

    return 0;    

}