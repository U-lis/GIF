#include "AniGIFMaker.h"
#include "GeoCalculator.h"
#include <exception>

using namespace Magick;
using std::exception;

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
	GeoCalculator calc(this->tolerance);

	string file=filename;
	string suffix=file.substr(file.rfind(".")+1);

	int inputGIFCnt=0;
	bool done=false;

	const Geometry initGeo(0, 0, 0, 0);
	Geometry imgGeo=initGeo;
	Geometry pageGeo=initGeo;

	while(!done) {
		try {
			if(inputGIFImage!=NULL) {
				cout << "NOT NULL" << endl;
				this->makeInputImage(inputGIFImage, inputGIFCnt, inputImage);
			} 
			else if (suffix.compare("gif")==0 || suffix.compare("GIF")==0) {
				cout << "GIF COMES" << endl;
				inputGIFImage=new GIF(filename);
				this->makeInputImage(inputGIFImage, inputGIFCnt, inputImage);
				cout << "inputImage Made : " << inputGIFCnt << endl;
			}
			else {
				cout << "ELSE" << endl;
				inputImage=new Image(filename);
			}

			calc.calcGeo(imgGeo, pageGeo, inputImage, (this->mode), (this->inputSize));
			this->setAniGIFProperties(inputImage);

			if(!this->isNoresize(imgGeo, pageGeo)) {
				inputImage->resize(imgGeo);
				inputImage->page(pageGeo);		
			}

			if(this->mode=='f') {
				this->cropImage(inputImage);
			}

			this->insertEndGIF(inputImage);
			imgGeo=initGeo;
			pageGeo=initGeo;

			if(cOption || inputGIFImage==NULL || inputGIFImage->getImgCnt()==inputGIFCnt) {
				cout << "done : true" << endl;
				done=true;
				if(inputGIFImage!=NULL) {
					delete inputGIFImage;
					inputGIFImage=NULL;
				}
			}
		}
		catch(const exception &e) {
			cout << "addToAniGIF Process Error : " << e.what() << endl;
			return;
		}
	}

	cout << filename << " Add Done : "  << this->getImgList() << endl;
}

void AniGIFMaker::makeAniGIF(const char *outfilename)
{
	char filename[100];
	if(outfilename!=NULL) {
		strncpy(filename, outfilename, 100);
	} 
	else {	//TODO
		sprintf(filename, "makeAniV3_%s%s%s_t%f.gif",  this->cOption ? "c" : "_", this->mode, this->inputSize, this->tolerance);
	}

	//DEBUG
	GIF merged(this->GIFImageList);
	merged.writeFile(filename, ALLIMGS);

	cout << "makeAniGIF Done" << endl;
}

void AniGIFMaker::makeInputImage(GIF *&inputGIFImage, int &cnt, Image *&inputImage)
{
	GIFBlobData* tmpStream=new GIFBlobData[inputGIFImage->getGIFSize(cnt)];
	inputGIFImage->writeBlob(tmpStream, inputGIFImage->getGIFSize(cnt));
	Blob* blob=new Blob(tmpStream, inputGIFImage->getGIFSize(cnt));
	inputImage=new Image(*blob);
	cnt++;
	if(blob!=NULL) {
		delete blob;
		blob=NULL;
	}
	if(tmpStream!=NULL) {
		delete[] tmpStream;
		tmpStream=NULL;
	}
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

void AniGIFMaker::cropImage(Image *&inputImage)
{
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

void AniGIFMaker::insertEndGIF(Image *&inputImage)
{
	Blob *endBlob=new Blob();
	inputImage->write(endBlob);
	this->GIFImageList.push_back(new GIF((GIFBlobData*)endBlob->data(), endBlob->length()));
	
	if(endBlob!=NULL) {
		delete endBlob;
		endBlob=NULL;
	}
	
	if(inputImage!=NULL) {
		delete inputImage;
		inputImage=NULL;
	}
}
