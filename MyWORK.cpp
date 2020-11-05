#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
const int BUFFER_SIZE = 1000;

void copySegment(int i, int segmentSize, FILE* fin, FILE* fout) {
}

int main(int argc, char* argv[]) {
    int i;
    sem_t* sem;
    pid_t pid;
    FILE* fin, * fout;
    struct stat sbuf;
    int fileSize, fileDescriptor, segmentSize;
    fin = fopen(argv[1], "r");
    fout = fopen("forkcopy.txt", "w");
    fileDescriptor = fileno(fin);
    fstat(fileDescriptor, &sbuf);
    fileSize = sbuf.st_size;
    segmentSize = fileSize / 4;
    sem = sem_open("sem", O_CREAT | O_EXCL, 0644, 4);
    sem_unlink("sem");
    for (i = 0; i < 4; i++) {
        pid = fork();
        if (pid < 0)
            printf("Fork error.\n");
        else if (pid == 0)
            break;
    }
    if (pid != 0) {
        while (pid = waitpid(-1, NULL, 0)) {
            if (errno == ECHILD)
                break;
        }
        sem_destroy(sem);
        fclose(fin);
        fclose(fout);
        exit(0);
    }
    else {
        sem_wait(sem);
        copySegment(i, segmentSize, fin, fout);
        sem_post(sem);
        exit(0);
    }
}