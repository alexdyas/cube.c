// -----------------------------------------------------------------------------
//
// File        - cube.c
//
// Description - Bouncing cube in text terminal
//
// Based on    - https://github.com/servetgulnaroglu/cube.c
//
// -----------------------------------------------------------------------------
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#ifndef _WIN32
    #include <unistd.h>
#else
    #include <windows.h>
    void usleep(__int64 usec)
    {
        HANDLE timer;
        LARGE_INTEGER ft;

        ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

        timer = CreateWaitableTimer(NULL, TRUE, NULL);
        SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
        WaitForSingleObject(timer, INFINITE);
        CloseHandle(timer);
    }
#endif

float A, B, C;

float cubeWidth = 7.0;
int screenWidth = 0;
int screenHeight = 0;
// We over-size these two to accomodate even the largest terminal size (number
// of characters) as we cannot size them dynamically
float zBuffer[100000];
char buffer[100000];
int backgroundASCIICode = ' ';
int distanceFromCam = 100;
float horizontalOffset = 0.0;
float verticalOffset = 0.0;
float K1 = 40;
float moveHorizontal = 0.5;
float moveVertical = 0.5;

float incrementSpeed = 0.6;

float x, y, z;
float ooz;
int xp, yp;
int idx;

float calculateX(int i, int j, int k) {
  return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
         j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

float calculateY(int i, int j, int k) {
  return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
         j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
         i * cos(B) * sin(C);
}

float calculateZ(int i, int j, int k) {
  return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {
  x = calculateX(cubeX, cubeY, cubeZ);
  y = calculateY(cubeX, cubeY, cubeZ);
  z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;

  ooz = 1 / z;

  xp = (int)(screenWidth / 2 + horizontalOffset + K1 * ooz * x * 2);
  yp = (int)(screenHeight / 2 + K1 * ooz * y);

  yp = yp + verticalOffset;

  idx = xp + yp * screenWidth;
  if (idx >= 0 && idx < screenWidth * screenHeight) {
    if (ooz > zBuffer[idx]) {
      zBuffer[idx] = ooz;
      buffer[idx] = ch;
    }
  }
}

int main() {
  float leftExtent = 0.0;
  float rightExtent = 0.0;
  float topExtent = 0.0;
  float bottomExtent = 0.0;

  // Init ncurses, grab screen size, and end it
  initscr();
  screenWidth = COLS;
  screenHeight = LINES;
  endwin();

  leftExtent = ( -(screenWidth / 2) + cubeWidth );
  rightExtent = ( screenWidth / 2 ) - cubeWidth;
  topExtent =  ( -( screenHeight / 2 ) + cubeWidth );
  bottomExtent = ( ( screenHeight / 2 ) - cubeWidth );

  printf("\x1b[2J");
  while (1) {
    memset(buffer, backgroundASCIICode, screenWidth * screenHeight);
    memset(zBuffer, 0, screenWidth * screenHeight * 4);

    if (horizontalOffset > rightExtent) {
        moveHorizontal = -0.5;
    }
    if (horizontalOffset < leftExtent) {
        moveHorizontal = 0.5;
    }
    horizontalOffset = horizontalOffset + moveHorizontal;

    if (verticalOffset < topExtent) {
        moveVertical = +0.5;
    }
    if (verticalOffset > bottomExtent) {
        moveVertical = -0.5;
    }
    verticalOffset = verticalOffset + moveVertical;

    for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
      for (float cubeY = -cubeWidth; cubeY < cubeWidth;
           cubeY += incrementSpeed) {
        calculateForSurface(cubeX, cubeY, -cubeWidth, '@');
        calculateForSurface(cubeWidth, cubeY, cubeX, '$');
        calculateForSurface(-cubeWidth, cubeY, -cubeX, '~');
        calculateForSurface(-cubeX, cubeY, cubeWidth, '#');
        calculateForSurface(cubeX, -cubeWidth, -cubeY, ';');
        calculateForSurface(cubeX, cubeWidth, cubeY, '+');
      }
    }

    printf("\x1b[H");
    for (int k = 0; k < screenWidth * screenHeight; k++) {
      putchar(k % screenWidth ? buffer[k] : 10);
    }

    A += 0.05;
    B += 0.05;
    C += 0.01;
    usleep(8000 * 2);
  }
  return 0;
}
