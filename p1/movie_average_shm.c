#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHM_KEY 0x1234

typedef struct {
    double avg1;
    double avg2;
} ShmData;

int main(int argc, char *argv[]) {
    // Kiểm tra số lượng tham số dòng lệnh
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ratings-file-1> <ratings-file-2>\n", argv[0]);
        return 1;
    }

    int shmid = shmget(SHM_KEY, sizeof(ShmData), 0644 | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget");
        return 1;
    }

    ShmData *shm = (ShmData*)shmat(shmid, NULL, 0);

    // Tiến trình con 1: Đọc file 1 từ argv[1]
    pid_t pid1 = fork();
    if (pid1 == 0) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file 1");
            exit(1);
        }
        int sum = 0, count = 0, rating;
        while (fscanf(file, "%*d %*d %d %*d", &rating) == 1) {
            sum += rating;
            count++;
        }
        fclose(file);
        shm->avg1 = (count > 0) ? (double)sum / count : 0.0;
        shmdt(shm);
        exit(0);
    }

    // Tiến trình con 2: Đọc file 2 từ argv[2]
    pid_t pid2 = fork();
    if (pid2 == 0) {
        FILE *file = fopen(argv[2], "r");
        if (!file) {
            perror("Error opening file 2");
            exit(1);
        }
        int sum = 0, count = 0, rating;
        while (fscanf(file, "%*d %*d %d %*d", &rating) == 1) {
            sum += rating;
            count++;
        }
        fclose(file);
        shm->avg2 = (count > 0) ? (double)sum / count : 0.0;
        shmdt(shm);
        exit(0);
    }

    // Tiến trình cha chờ 2 tiến trình con kết thúc
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("Average from %s: %.2f\n", argv[1], shm->avg1);
    printf("Average from %s: %.2f\n", argv[2], shm->avg2);

    // Dọn dẹp Shared Memory trước khi thoát
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
