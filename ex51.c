/*
 * Gil Kagan
 * 315233221
 */

#include <stdio.h>
#include <signal.h>
#include <termio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define ERROR "Error in system call"
#define ERROR_SIZE strlen(ERROR)
#define FILE_TO_RUN "./draw.out"
#define EXIT 'q'

int writeToPipeFileDes = -1;

void error() {
    write(STDERR_FILENO, ERROR, ERROR_SIZE);
}
void chldSignalHandler(int sigNum, siginfo_t * info, void* ptr) {
    if (writeToPipeFileDes != -1){
        // closes the pipe write file descriptor.
        if (close(writeToPipeFileDes) < 0)
            error();
    }
    exit(EXIT_SUCCESS);
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");

    return (buf);
}


void execute() {
    int pipedes[2];
    if (pipe(pipedes) < 0)
        error();
    writeToPipeFileDes = pipedes[1];
    struct sigaction chldAction;
    chldAction.sa_sigaction = chldSignalHandler;
    chldAction.sa_flags = SA_SIGINFO;
    if (sigaction(SIGCHLD, &chldAction, NULL) < 0)
        error();
    pid_t pid;
    switch ((pid=fork())) {
        case -1:
            error();
            exit(EXIT_FAILURE);
            // chld case - read from the pipe.
        case 0:
            if (close(pipedes[1]) < 0)
                error();
            if (dup2(pipedes[0], STDIN_FILENO) < 0)
                error();
            char * args[] = {FILE_TO_RUN, NULL};
            execv("draw.out", args);
            error();
            exit(EXIT_FAILURE);

         // parent case - write to pipe.
        default:
            if (close(pipedes[0]) < 0)
                error();
            char key[1] = {' '};
            while (*key != EXIT) {
                *key = getch();
                if (write(pipedes[1], key, 1) < 0)
                    error();
                if (kill(pid, SIGUSR2) < 0)
                    error();
            }
            // write the end char.
            if (write(pipedes[1], key, 1) < 0)
                error();
            // wait for the child to send a signal that he finished, and handle it.
            pause();
    }
}

int main() {
    execute();
    return 0;
}