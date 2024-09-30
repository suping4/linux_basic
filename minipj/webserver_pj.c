#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <wiringPi.h>

#include "minipj1_web.c"

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE];
    recv(client_socket, buffer, BUFFER_SIZE, 0);

    char method[10], path[100];
    sscanf(buffer, "%s %s", method, path);

    if (strcmp(path, "/") == 0) {
        // Serve the main page
        char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                         "<html><head><title>Hardware Control</title></head>"
                         "<body>"
                         "<h1>Hardware Control</h1>"
                         "<button onclick='toggleAlarm()'>Toggle Alarm</button>"
                         "<p id='alarmStatus'></p>"
                         "<p id='ledStatus'></p>"
                         "<script>"
                         "function toggleAlarm() {"
                         "  fetch('/toggle-alarm').then(() => updateStatus());"
                         "}"
                         "function updateStatus() {"
                         "  fetch('/status').then(response => response.json())"
                         "    .then(data => {"
                         "      document.getElementById('alarmStatus').textContent = 'Alarm: ' + (data.alarm ? 'On' : 'Off');"
                         "      document.getElementById('ledStatus').textContent = 'LED: ' + (data.led ? 'On' : 'Off');"
                         "    });"
                         "}"
                         "setInterval(updateStatus, 1000);"
                         "updateStatus();"
                         "</script>"
                         "</body></html>";
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(path, "/toggle-alarm") == 0) {
        // Toggle the alarm
        setAlarm(!getAlarmState());
        char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nOK";
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(path, "/status") == 0) {
        // Return the current status
        char response[200];
        snprintf(response, sizeof(response),
                 "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"
                 "{\"alarm\": %s, \"led\": %s}",
                 getAlarmState() ? "true" : "false",
                 getLedState() ? "true" : "false");
        send(client_socket, response, strlen(response), 0);
    } else {
        // 404 Not Found
        char *response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found";
        send(client_socket, response, strlen(response), 0);
    }

    close(client_socket);
}

int main() {
    initHardware();

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 10) == 0) {
        printf("Listening on port %d...\n", PORT);
    } else {
        perror("Listening failed");
        exit(1);
    }

    while (1) {
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
        
        pthread_t thread;
        if (pthread_create(&thread, NULL, (void*)handle_request, (void*)(intptr_t)client_socket) != 0) {
            perror("Failed to create thread");
            continue;
        }
        pthread_detach(thread);
    }

    return 0;
}