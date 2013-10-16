#ifndef ANI_GIF_MAKER_H
#define ANI_GIF_MAKER_H

#include <iostream>
#include <string>
#include <list>
#include <Magick++.h>

#include "GIFClass.h"

using std::cout;
using std::endl;
using std::string;
using std::list;

using namespace Magick;

class AniGIFMaker{
	private:
		double tolerance;
		bool cOption;
		char mode;
		char *inputSize;
		int sizeWidth;
		int sizeHeight;
		list<GIF*> GIFImageList;

	public:
		AniGIFMaker(double tolerance, bool cOption, char mode, char *inputSize);
		~AniGIFMaker();

		bool addToAniGIF(char* input);
		void makeAniGIF();

};

#endif	// ANI_GIF_MAKER_H
