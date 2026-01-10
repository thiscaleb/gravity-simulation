#ifndef MATRIX4_H
#define MATRIX4_H

// 4x4 matrix represented as a 2d array
// This will is technically row-major, but OpenGL will properly register it
typedef float matrix4 [4][4];

extern const float identityMatrix4[16];

#endif