/*
 * Gil Kagan
 * 315233221
 */


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define ERROR "Error in system call"
#define ERROR_SIZE strlen(ERROR)
#define SIZE 20
#define SHAPE_SIZE 3


struct position{
    int row,col;
};

typedef struct Shape {
    // b is the middle of the shape.
    struct position pos[SHAPE_SIZE];
} Shape;

Shape shape;
char screen[SIZE][SIZE];

void error() {
    write(STDERR_FILENO, ERROR, ERROR_SIZE);
}


void setBorders() {
    int i;
    for (i = 0; i < SIZE; i++) {

        screen[i][0] = '*';
        screen[i][SIZE - 1] = '*';
    }
    for (i = 0; i < SIZE; i++) {
        screen[SIZE - 1][i] = '*';
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
            if (write(STDOUT_FILENO, &(screen[i][j]), sizeof(screen[i,j])) < 0) error();
        }
        if (write(STDOUT_FILENO, "\n", 1) < 0) error();
    }
}

void addShapeToScreen() {
    int i;
    for (i = 0; i < SHAPE_SIZE; i++) {
        screen[shape.pos[i].row][shape.pos[i].col] = '-';
    }
}


void removeShapeFromScreen() {
    int i;
    for (i = 0; i < SHAPE_SIZE; i++) {
        screen[shape.pos[i].row][shape.pos[i].col] = ' ';
    }
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
    addShapeToScreen();
}

int main() {
    printf("Hello, World!\n");
    return 0;
}