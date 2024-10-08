#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

int fndControl(){
    int i;
    int gpioPins[4] = {4,1,16,15};
    int number[10][4] = {
        {0,0,0,0},
        {0,0,0,1},
        {0,0,1,0},
        {0,0,1,1},
        {0,1,0,0},
        {0,1,0,1},
        {0,1,1,0},
        {0,1,1,1},
        {1,0,0,0},
        {1,0,0,1}
    };

    for(i=0;i<4;i++){
        pinMode(gpioPins[i], OUTPUT);
    }

    for(int j=0; j<10; j++){
        for(i=0;i<4;i++){
            digitalWrite(gpioPins[i], number[j][i]?HIGH:LOW);
        }
        delay(1000);
    }
    
    // getchar();

    for(int i=0; i<4; i++){
        digitalWrite(gpioPins[i], HIGH);
    }

    return 0;
}

int main(int argc, char **argv){
    int no;

    // if(argc < 2) {
    //     printf("Usage : %s number\n", argv[0]);
    //     return -1;
    // }

    wiringPiSetup();
    fndControl();
    return 0;
}
