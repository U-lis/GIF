#include "AniGIFMaker.h"
#include "GeoCalculator.h"

using namespace Magick;

AniGIFMaker::AniGIFMaker(double _tolerance, bool _cOption, char _mode, char *_inputSize)
	: tolerance(3), cOption(false), mode('m'), inputSize(NULL), maxGeo(0, 0, 0, 0)
{
	tolerance=_tolerance;
	cOption=_cOption;
	mode=_mode;
	inputSize=_inputSize;
}

AniGIFMaker::~AniGIFMaker()
{
	list<GIF*>::iterator itr=GIFImageList.begin();
	for(; itr!=GIFImageList.end(); itr++) {
		delete *itr;
	}
}

void AniGIFMaker::addToAniGIF(const char* filename)
{
	GIF *inputGIFImage=NULL;
	Image *inputImage=NULL;

	string file=filename;
	string suffix=file.substr(file.rfind(".")+1);

	if(suffix.compare("gif")==0 || suffix.compare("GIF")==0) {
		inputGIFImage=new GIF(filename);
		GIFData* tmpStream=new GIFData[inputGIFImage->getGIFSize(0)];
		inputGIFImage->writeBlob(tmpStream, inputGIFImage->getGIFSize(0));
		inputImage=new Image((char*)tmpStream);
		delete[] tmpStream;
	} else {
		inputImage=new Image(filename);
	}

	//calcSize, resize+crop, push_back
	Geometry imgGeo(0, 0);
	Geometry pageGeo(0, 0);
	
	GeoCalculator calc(this->tolerance);
	calc.calcGeo(imgGeo, pageGeo, inputImage, (this->mode), (this->inputSize));
	this->setAniGIFProperties(inputImage);
	
	if(!this->isNoresize(imgGeo, pageGeo)) {
		inputImage->resize(imgGeo);
		inputImage->page(pageGeo);		
	}

	if(this->mode=='f') {
		try {
			Geometry cropGeo(this->inputSize);
			cropGeo.xOff();
			cropGeo.yOff();
			inputImage->crop(cropGeo);
			cropGeo.xOff();
			cropGeo.yOff();
			inputImage->page(cropGeo);
		} catch(exception e) {
			cout << "Crop Error : " << e.what() << endl;
		}
	}

	if(this->maxGeo.width() < inputImage->page().width())
		this->maxGeo.width(inputImage->page().width());
	if(this->maxGeo.height() < inputImage->page().height())
		this->maxGeo.height(inputImage->page().height());

	Blob *endBlob=new Blob();
	inputImage->write(endBlob);
	this->GIFImageList.push_back(new GIF((GIFData*)endBlob->data(), endBlob->length()));
	
	if(endBlob!=NULL) {
		delete endBlob;
		endBlob=NULL;
	}
	if(inputImage!=NULL) {
		delete inputImage;
		inputImage=NULL;
	}

	//TODO
	if(!this->cOption) {
		for(int i=1; i<inputGIFImage->getImageCnt(); i++)
		{
			//make next Iamge, calcSize, resize+crop, push_back
		}
	}
	cout << filename << " Add Done" << endl;
}

void AniGIFMaker::makeAniGIF(const char *outfilename)
{
	char filename[100];
	if(outfilename!=NULL) {
		strncpy(filename, outfilename, 100);
	} else {	//TODO
		sprintf(filename, "makeAniV3_%s%s%s_t%f.gif",  this->cOption ? "c" : "_", this->mode, this->inputSize, this->tolerance);
	}

	//DEBUG
	GIF merged(this->GIFImageList);
	merged.writeFile(filename, ALLIMGS);

	cout << "makeAniGIF Done" << endl;
}

void AniGIFMaker::setAniGIFProperties(Image *&image)
{
	image->animationIterations(0);
	if(image->magick()!="GIF" || this->cOption) {
		image->gifDisposeMethod(2);
		image->animationDelay(100);
	}
	image->magick("GIF");
}

bool AniGIFMaker::isNoresize(const Geometry imgGeo, const Geometry pageGeo)
{
	return ((imgGeo.width()==0 && imgGeo.height()==0) || (pageGeo.width()==0 && pageGeo.height()==0));
}
