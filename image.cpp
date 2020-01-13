#include "image.h"
#include "bmpIO.h"

//----------------------------------------------------------------------------
Image::Image(int w, int h)
{
	resize(w, h);
}
//----------------------------------------------------------------------------
Image::Image(char *filename)
{
	load(filename);
}
//----------------------------------------------------------------------------
Image::~Image()
{
}
//----------------------------------------------------------------------------
void Image::resize(int w, int h)
{
	width = w;
	height = h;
	if(w*h > 0) pixel.resize(width*height);
}
//----------------------------------------------------------------------------
int Image::getWidth()
{
	return(width);
}
//----------------------------------------------------------------------------
int Image::getHeight()
{
	return(height);
}
//----------------------------------------------------------------------------
Colorub Image::getPixel(int i, int j)
{
	return(pixel[width*j+i]);
}
//----------------------------------------------------------------------------
void Image::setPixel(int i, int j, Colorub c)
{
	pixel[width*j+i] = c;
}
//----------------------------------------------------------------------------
bool Image::save(char *filename)
{
	vector<unsigned char> tmp_pixel(width*height*3);
	int k=0;
	for(int i=0; i<width*height; i++) {
		tmp_pixel[k + 0] = pixel[i].b;
		tmp_pixel[k + 1] = pixel[i].g;
		tmp_pixel[k + 2] = pixel[i].r;
		k += 3;
	}
	return(writeBmpImage(filename, width, height, 3, &(tmp_pixel[0])));
}
//----------------------------------------------------------------------------
bool Image::load(char *filename)
{
	unsigned char* tmp_pixel = NULL;
	int w, h, nc;
	bool flag;
	if((flag = readBmpImage(filename, &w, &h, &nc, &tmp_pixel))) {
		resize(w, h);
		int k = 0;
		for(int i=0; i<width*height; i++) {
			pixel[i].b = tmp_pixel[k + 0];
			pixel[i].g = tmp_pixel[k + 1];
			pixel[i].r = tmp_pixel[k + 2];
			k += 3;
		}
		delete [] tmp_pixel;
	}

	return(flag);
}
//----------------------------------------------------------------------------