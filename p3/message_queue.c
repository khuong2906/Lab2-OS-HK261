#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

#define MSG_SIZE 100
#define CLIENT_TO_SERVER 1234

typedef struct {
    long mtype;
    char text[MSG_SIZE];
} Message;

int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        return 1;
    }

    int msgid = msgget(CLIENT_TO_SERVER, 0666 | IPC_CREAT);

    pid_t pid = fork();
    if (pid == 0) {
        // Tiến trình con (Client): Nhận tin nhắn từ queue
        Message msg;
        if (msgrcv(msgid, &msg, sizeof(msg.text), 0, 0) > 0) {
            printf("[Client] Received: %s", msg.text);
        }
        exit(0);
    } else {
        // Tiến trình cha (Server): Gửi tin nhắn vào queue
        Message msg;
        msg.mtype = 1;
        snprintf(msg.text, MSG_SIZE, "Hello from Server\n");
        msgsnd(msgid, &msg, sizeof(msg.text), 0);

        wait(NULL);
        // Dọn dẹp tài nguyên Message Queue trước khi kết thúc
        msgctl(msgid, IPC_RMID, NULL);
    }
    return 0;
}
