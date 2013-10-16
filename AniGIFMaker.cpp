#include "AniGIFMaker.h"

using namespace Magick;

AniGIFMaker::AniGIFMaker(double tolerance, bool cOption, char mode, char *inputSize)
	: tolerance(3), cOption(false), mode('m'), inputSize(NULL), sizeWidth(0), sizeHeight(0)
{

}

AniGIFMaker::~AniGIFMaker()
{
	list<GIF*>::iterator itr=imageList.begin();
	for(; itr!=imageList.end(); itr++) {
		delete *itr;
	}
}

bool AniGIFMaker::addToAniGIF(char* filename)
{
	GIF *inputGIFImage=NULL;
	Image *inputImage=NULL;

	string file=filename;
	string suffix=file.substr(file.rfind(".")+1);

	if(suffix.compare("gif")==0 || suffix.compare("GIF")==0) {
		inputGIFImage=new GIF(filename);
		GIFData* tmpStream=new GIFData[inputGIFImage.getGIFSize(0)];
		GIFData.writeBlob(tmpStream, 0);
		inputImage=new Iamge(tmpStream);
		delete[] tmpStream;
	} else {
		inputImage=new Image(filename);
	}

	//calcSize, resize+crop, push_back

	if(!this->cOption) {
		for(int i=1; i<inputGIFImage->getImageCnt(); i++)
		{
			//make next Iamge, calcSize, resize+crop, push_back
		}
	}

	cout << filename << " Add Done" << endl;
}

void AniGIFMaker::makeAniGIF()
{
	char filename[100];
	sprintf(filename, "makeAniV3_%s%s%s_t%f.gif",  this->cOption ? "c" : "", this->mode, this->inputSize, this->tolerance);

	//DEBUG
	cout << filename << endl;

	GIF merged(this->imageList);
	merged.writeFile(filename, ALLIMGS);

	cout << "makeAniGIF Done" << endl;
}

GIF AniGIFMaker::makeImages(char* filename)
{
	string file=filename;
	string suffix=file.sbstr(file.rfind(".")+1);

	if(suffix.compare("gif")==0 || suffix.compare("GIF")==0) {
		return GIF(filename);
	} else {
		return NULL;
	}
}
