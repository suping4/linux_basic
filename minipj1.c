#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <softTone.h>
#include <signal.h>

#define SPKR 6
#define SW 5
#define LED 1
#define CDS 0

int notes[] = {361, 440, 0};
volatile int isAlarm = 0;
volatile int running = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *musicPlay(void *arg) {
    softToneCreate(SPKR);
    while (running) {
        pthread_mutex_lock(&mutex);
        if (isAlarm) {
            for (int i = 0; i < 3; i++) {
                if(!isAlarm) break;
                softToneWrite(SPKR, notes[i]);
                delay(280);
            }
        } else {
            softToneWrite(SPKR, 0);
        }
        pthread_mutex_unlock(&mutex);
        delay(10);
    }
    softToneWrite(SPKR, 0);
    softToneStop(SPKR);
    return NULL;
}

void *switchControl(void *arg) {
    while (running) {
        if (digitalRead(SW) == LOW) {
            pthread_mutex_lock(&mutex);
            isAlarm = !isAlarm;
            pthread_mutex_unlock(&mutex);
            delay(200);  // 디바운싱
        }
        delay(10);
    }
    return NULL;
}

void *cdsControl(void *arg) {
    while (running) {
        if (digitalRead(CDS) == HIGH) {
            digitalWrite(LED, LOW);
        } else {
            digitalWrite(LED, HIGH);
        }
        delay(10);
    }
    return NULL;
}

void cleanup() {
    running = 0;
    digitalWrite(LED, LOW);
    softToneWrite(SPKR, 0);
    softToneStop(SPKR);
    pthread_mutex_destroy(&mutex);
}

void sigintHandler(int sig_num) {
    printf("\n프로그램을 종료합니다...\n");
    cleanup();
    exit(0);
}

int main() {
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "wiringPiSetup 초기화 실패\n");
        return -1;
    }

    pinMode(SW, INPUT);
    pinMode(CDS, INPUT);
    pinMode(LED, OUTPUT);

    signal(SIGINT, sigintHandler);

    pthread_t switch_thread, cds_thread, music_thread;

    if (pthread_create(&switch_thread, NULL, switchControl, NULL) != 0 ||
        pthread_create(&cds_thread, NULL, cdsControl, NULL) != 0 ||
        pthread_create(&music_thread, NULL, musicPlay, NULL) != 0) {
        fprintf(stderr, "쓰레드 생성 실패\n");
        cleanup();
        return -1;
        }

    pthread_join(switch_thread, NULL);
    pthread_join(cds_thread, NULL);
    pthread_join(music_thread, NULL);

    cleanup();
    return 0;
}
