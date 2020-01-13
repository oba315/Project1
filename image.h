#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>


using namespace std;

typedef struct {
	unsigned char r, g, b;
} Colorub;

class Image {
private:
	int			   width, height;
	vector<Colorub>	pixel;

public:
	Image(int w = 0, int h = 0);
	Image(char *filename);
	~Image();

	void		resize(int w, int h);
	int			getWidth();
	int			getHeight();
	Colorub		getPixel(int i, int j);
	void		setPixel(int i, int j, Colorub c);

	bool		load(char *filename);
	bool		save(char *filename);
};


#endif
