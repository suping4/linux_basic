#include <wiringPi.h>
#include <softTone.h>

#define SPKR 6
#define TOTAL 32

int notes[] = {391, 391, 440, 440, 391, 391, 329,329,391,391,329,329,293,293,293,0,391,391,440,440,391,391,329,329,391,329, 293,329,261,261,261};

int musicPlay(){
    int i;

    softToneCreate(SPKR);
    for(i=0; i<TOTAL; i++){
        softToneWrite(SPKR, notes[i]);
        delay(280);
    }
    return 0;
}

int main(int argc, char *argv[]){
    wiringPiSetup();
    musicPlay();
    return 0;
}