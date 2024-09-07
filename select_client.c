#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 5100       // 서버 포트 번호

int main(int argc, char** argv) {
    int sock;
    struct sockaddr_in servaddr;
    char mesg[BUFSIZ]; // 메시지를 저장할 버퍼

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    // 서버 주소 구조체 초기화
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // 주소 패밀리 설정 (IPv4)
    servaddr.sin_port = htons(SERVER_PORT); // 포트 설정 (네트워크 바이트 순서로 변환)

    // 서버 IP 주소 변환
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("inet_pton()");
        return -1;
    }

    // 서버에 연결
    if (connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect()");
        return -1;
    }

    fd_set readfds; // 읽기 파일 디스크립터 집합

    printf("서버에 연결되었습니다. 메시지를 입력하세요 (종료하려면 'q' 입력):\n");

    while (1) {
        // 파일 디스크립터 초기화
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds); // 소켓을 읽기 집합에 추가
        FD_SET(0, &readfds); // 0은 표준 입력(stdin)

        // select 호출
        int maxfd = sock + 1; // 최대 파일 디스크립터 (소켓 + 1)
        if (select(maxfd, &readfds, NULL, NULL, NULL) < 0) {
            perror("select()");
            break;
        }

        // 서버로부터 데이터 수신
        if (FD_ISSET(sock, &readfds)) {
            memset(mesg, 0, sizeof(mesg)); // 메시지 버퍼 초기화
            int n = read(sock, mesg, sizeof(mesg)); // 서버로부터 데이터 읽기
            if (n > 0) {
                printf("서버로부터 수신한 데이터: %s\n", mesg); // 수신한 데이터 출력
            } else {
                printf("서버와의 연결이 끊어졌습니다.\n");
                break;
            }
        }

        // 사용자 입력 처리
        if (FD_ISSET(0, &readfds)) {
            memset(mesg, 0, sizeof(mesg)); // 메시지 버퍼 초기화
            fgets(mesg, sizeof(mesg), stdin); // 사용자 입력 받기

            // 'q' 입력 시 종료
            if (strncmp(mesg, "q", 1) == 0) {
                break;
            }

            // 서버에 메시지 전송
            write(sock, mesg, strlen(mesg)); // 서버에 데이터 쓰기
        }
    }

    close(sock); // 소켓 닫기
    return 0;
}
