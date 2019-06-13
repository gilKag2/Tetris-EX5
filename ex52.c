/*
 * Gil Kagan
 * 315233221
 */


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>

#define ERROR "Error in system call"
#define ERROR_SIZE strlen(ERROR)
#define SIZE 20
#define SHAPE_SIZE 3
#define HORIZONTAL 1
#define BORDER '*'
#define SHAPE '-'
#define ALARM_TIME 1
#define LEFT 'a'
#define DOWN 's'
#define ROTATE 'w'
#define RIGHT 'd'
#define EXIT 'q'



struct position{
    int row,col;
};

typedef struct Shape {
    // b is the middle of the shape.
    struct position pos[SHAPE_SIZE];
    bool state;
    int midIndex;
} Shape;

Shape shape;
char screen[SIZE][SIZE];

bool shouldRun;

void error() {
    write(STDERR_FILENO, ERROR, ERROR_SIZE);
}


void exitGame() {
    shouldRun = false;
    if (close(STDIN_FILENO) < 0)
        error();
    exit(EXIT_SUCCESS);
}

bool validateMove(short pos) {
    return pos < SIZE;
}

void setBorders() {
    int i;
    for (i = 0; i < SIZE; i++) {
        screen[i][0] = BORDER;
        screen[i][SIZE - 1] = BORDER;
    }
    for (i = 0; i < SIZE; i++) {
        screen[SIZE - 1][i] = BORDER;
    }
}

void resetScreen() {
    int i,j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            screen[i][j] = ' ';
        }
    }
}

void drawScreen() {
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (write(STDOUT_FILENO, &(screen[i][j]), sizeof(screen[i][j])) < 0)
                error();
        }
        if (write(STDOUT_FILENO, "\n", 1) < 0)
            error();
    }
}

void addShapeToScreen() {
    int i;
    for (i = 0; i < SHAPE_SIZE; i++) {
        screen[shape.pos[i].row][shape.pos[i].col] = SHAPE;
    }
}


void removeShapeFromScreen() {
    int i;
    for (i = 0; i < SHAPE_SIZE; i++) {
        screen[shape.pos[i].row][shape.pos[i].col] = SHAPE;
    }
}

void rotateShape() {
    // change to vertical.
    if (shape.state == HORIZONTAL) {

        // if we cant rotate, dont do anything.
        if (!validateMove(shape.pos[shape.midIndex].row + (SHAPE_SIZE / 2)))
            return;

        // first we remove the shape to draw the new one.
        removeShapeFromScreen();

        int i, mid = shape.midIndex;
        // sets the starting row such that they will align with the middle one.
        int row = shape.pos[mid].row - (SHAPE_SIZE / 2);
        for (i = 0; i < SHAPE_SIZE; i++) {
            if (i == mid) continue;
            // move past the middle element.
            if (row == shape.pos[mid].row) row++;

            shape.pos[i].col = shape.pos[mid].col;
            shape.pos[i].row = row++;
        }
        // add the shape back to the screen.
        addShapeToScreen();
     // change to horizontal.
    } else {

        // when changing to horizontal, we need to check both left and right
        if (!validateMove(shape.pos[shape.midIndex].col + (SHAPE_SIZE / 2)) ||
            !validateMove(shape.pos[shape.midIndex].col - (SHAPE_SIZE / 2)))
            return;

        /*
         * this part is similar to the case of horizontal, we just switched the col and row,
         */
        removeShapeFromScreen();
        int i, mid = shape.midIndex;
        int col = shape.pos[mid].col - (SHAPE_SIZE / 2);
        for (i = 0; i < SHAPE_SIZE; i++) {
            if (i == mid) continue;
            if (col == shape.pos[mid].col) col++;

            shape.pos[i].row = shape.pos[mid].row;
            shape.pos[i].col = col++;
        }
        addShapeToScreen();
    }
}
void moveBy(int move) {
    // validate that we can move left or right.
    if (move < 0) {
        if (!validateMove(shape.pos[0].col + move))
            return;
    } else if (!validateMove(shape.pos[SHAPE_SIZE - 1].col + move))
        return;

    removeShapeFromScreen();

    int i;
    for (i = 0; i < SHAPE_SIZE; i++) {
        shape.pos[i].col += move;
    }
    addShapeToScreen();
}


void initShape() {
    int i;
    for (i = 0; i < SHAPE_SIZE; i++) {
        shape.pos[i].row = 0;
    }
    // sets the starting col position such that the middle if the shape will be in the middle of the screen.
    int col = (SIZE / 2) - SHAPE_SIZE + (SHAPE_SIZE / 2);
    for (i = 0; i < SIZE; i++) {
        shape.pos[i].col = col++;
    }
    shape.state = HORIZONTAL;
    shape.midIndex = SHAPE_SIZE / 2;
    addShapeToScreen();
}

void moveDown() {

    //if we  cant move down, then we reached the bottom of the screen.
    if (!validateMove(shape.pos[SHAPE_SIZE - 1].row)){
        removeShapeFromScreen();
        initShape();
        return;
    }

    removeShapeFromScreen();

    int i;
    for (i = 0; i < SHAPE_SIZE; i++) {
        shape.pos[i].row += 1;
    }
    addShapeToScreen();
}

void userSignalHandler(int sigNum, siginfo_t* siginfo, void* ptr) {
    char key[1];
    if (read(STDIN_FILENO, key, 1) < 0)
        error();

    switch (*key) {
        case EXIT:
            resetScreen();
            exitGame();
            break;
        case RIGHT:
            moveBy(1);
            break;
        case LEFT:
            moveBy(-1);
            break;
        case DOWN:
            moveDown();
            break;
        case ROTATE:
            rotateShape();
            break;
        default:
            break;
    }
    drawScreen();
}


void alarmSignalHandler(int sigNum, siginfo_t* siginfo, void* ptr){
    system("clear");
    moveDown();
    drawScreen();
    alarm(ALARM_TIME);
}


int main() {
    system("clear");
    shouldRun = true;
    struct sigaction userAct, alarmAct;
    userAct.sa_sigaction = userSignalHandler;
    alarmAct.sa_sigaction = alarmSignalHandler;
    userAct.sa_flags = alarmAct.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR2, &userAct, NULL) < 0)
        error();
    if (sigaction(SIGALRM, &alarmAct, NULL) < 0)
        error();

    setBorders();
    drawScreen();
    initShape();
    alarm(ALARM_TIME);
    while (shouldRun)
        pause();

    return 0;
}