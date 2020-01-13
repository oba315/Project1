#ifndef BMP_IO_H__
#define BMP_IO_H__
#include <Windows.h>
#include <stdio.h>

bool readBmpImage(char *filename, int *width, int *height, int *nchannel, unsigned char **pixel);
bool writeBmpImage(char* filename, int width, int height, int nchannel, unsigned char *pixel);

#endif
