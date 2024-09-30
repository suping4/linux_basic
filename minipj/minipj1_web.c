#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <softTone.h>
#include <signal.h>
#include <stdbool.h>

#define SPKR 6
#define SW 5
#define LED 1
#define CDS 0

int notes[] = {361, 440, 0};
volatile bool isAlarm = false;
volatile bool running = true;
volatile bool ledOn = false;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *musicPlay(void *arg) {
    softToneCreate(SPKR);
    while (running) {
        pthread_mutex_lock(&mutex);
        if (isAlarm) {
            for (int i = 0; i < 3 && isAlarm; i++) {
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
        if (digitalRead(CDS) == LOW) {
            digitalWrite(LED, LOW);
            ledOn = false;
        } else {
            digitalWrite(LED, HIGH);
            ledOn = true;
        }
        delay(10);
    }
    return NULL;
}

void cleanup() {
    running = false;
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

// 웹서버에서 호출할 함수들
void setAlarm(bool state) {
    pthread_mutex_lock(&mutex);
    isAlarm = state;
    pthread_mutex_unlock(&mutex);
}

bool getAlarmState() {
    return isAlarm;
}

bool getLedState() {
    return ledOn;
}

void initHardware() {
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "wiringPiSetup 초기화 실패\n");
        exit(1);
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
        exit(1);
    }
}

