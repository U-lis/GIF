#ifndef ANI_GIF_MAKER_H
#define ANI_GIF_MAKER_H

#include <iostream>
#include <string>
#include <list>
#include <Magick++.h>

#include "GIF.h"

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
		Geometry maxGeo;
		list<GIF*> GIFImageList;
		
		void calcSize(Geometry imgGeo, Geometry pageGeo);
		void setAniGIFProperties(Image *&image);
		bool isNoresize(const Geometry imgGeo, const Geometry pageGeo);
		void makeInputImage(GIF *&inputGIFImage, int &cnt, Image *&inputImage);
		void cropImage(Image *&inputImage);
		void insertEndGIF(Image *&inputImage);

	public:
		AniGIFMaker(double tolerance, bool cOption, char mode, char *inputSize);
		~AniGIFMaker();

		void addToAniGIF(const char* input);
		void makeAniGIF(const char *outfilename);
		int getImgList(){return GIFImageList.size();}

};

#endif	// ANI_GIF_MAKER_H
