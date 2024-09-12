#ifndef ROTATE_H
#define ROTATE_H

#include <cstddef> // for size_t

typedef unsigned char BYTE;

BYTE Interpolate(BYTE f[], int sz[], int m, int n, float ex, float ey, char way);
BYTE* normalRoate(BYTE img[], int w, int h, double theta, int* neww, int* newh);
BYTE* DDARoateFast(BYTE img[], int w, int h, double theta, int* neww, int* newh);

#endif // ROTATE_H
