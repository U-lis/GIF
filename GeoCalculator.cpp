#include "GeoCalculator.h"

using std::cout;
using std::endl;

int inline calcSize(int size, double ratio) {return int(double(size)*ratio);}

GeoCalculator::GeoCalculator(double _tolerance)
	: tolerance(3), geoWidth(-1), geoHeight(-1)
{
	tolerance=_tolerance;
}

GeoCalculator::~GeoCalculator()
{
}

void GeoCalculator::calcGeo(Geometry &imgGeo, Geometry &pageGeo, Image *&image, char &mode, char *&inputSize)
{
	double ratio=0;

	this->parseInputSize(mode, inputSize);
	if(this->geoWidth==0 || this->geoHeight==0) {
		cout << "Incompatible inputSize Error. Exiting..." << endl;
		exit(EXIT_FAILURE);
	} else {
		this->maxRatio=double(image->page().width())/double(image->page().height())*this->tolerance;
		this->minRatio=double(image->page().width())/double(image->page().height())/this->tolerance;

		switch(mode) {
			case 'w':
				this->setWidthOption(image, imgGeo, pageGeo);
				this->setNoresize(image, imgGeo, pageGeo);
				break;
			case 'h':
				this->setHeightOption(image, imgGeo, pageGeo);
				this->setNoresize(image, imgGeo, pageGeo);
				break;
			case 'a':
				this->setAutoOption(image, imgGeo, pageGeo);
				break;
			default:
				cout << "Imcompatible Resize Mode Error. Set to Default Mode 'm'" << endl;
			case 'm':
				this->setMobileOption(image, imgGeo, pageGeo);
				this->setNoresize(image, imgGeo, pageGeo);
				break;
			case 'f':
				this->setFixedOption(image, imgGeo, pageGeo);
				break;
		}
	}
}

void GeoCalculator::parseInputSize(const char mode, const char *inputSize)
{
	strncpy(this->inputSize, inputSize, 20);
	char input[20];
	strncpy(input, this->inputSize, 20);
	char *param=strtok(input, "x");
	this->geoWidth=atoi(param);
	if(mode=='h')
		this->geoHeight=this->geoWidth;
	param=strtok(NULL, "x");
	if(param!=NULL) {
		this->geoHeight=atoi(param);
	}
}

void GeoCalculator::setWidthOption(Image *&image, Geometry &imgGeo, Geometry &pageGeo)
{
	double ratio=double(geoWidth)/double(image->page().width());
	imgGeo=Geometry(calcSize(image->columns(), ratio), 0);
	pageGeo=Geometry(calcSize(image->page().width(), ratio), calcSize(image->page().height(), ratio), 
			calcSize(image->page().xOff(), ratio), calcSize(image->page().yOff(), ratio));
}

void GeoCalculator::setHeightOption(Image *&image, Geometry &imgGeo, Geometry &pageGeo)
{
	double ratio=double(geoHeight)/double(image->page().height());
	imgGeo=Geometry(0, calcSize(image->rows(), ratio));
	pageGeo=Geometry(calcSize(image->page().width(), ratio), calcSize(image->page().height(), ratio), 
			calcSize(image->page().xOff(), ratio), calcSize(image->page().yOff(), ratio));

}

void GeoCalculator::setFixedOption(Image *&image, Geometry &imgGeo, Geometry &pageGeo)
{
	double ratio=0;
	if(this->geoWidth/image->page().width() > this->geoHeight/image->page().height()) {
		ratio=double(geoWidth)/double(image->page().width());
		imgGeo=Geometry(calcSize(image->columns(), ratio), 0);
	} else {
		ratio=double(geoHeight)/double(image->page().height());
		imgGeo=Geometry(0, calcSize(image->rows(), ratio));
	}
	pageGeo=Geometry(calcSize(image->page().width(), ratio), calcSize(image->page().height(), ratio), 
			calcSize(image->page().xOff(), ratio), calcSize(image->page().yOff(), ratio));
}

void GeoCalculator::setMobileOption(Image *&image, Geometry &imgGeo, Geometry &pageGeo)
{
	double ratio=0;
	if(image->page().width()/this->geoWidth > image->page().height()/this->geoHeight) {
		ratio=double(geoWidth)/double(image->page().width());
		imgGeo=Geometry(calcSize(image->columns(), ratio), 0);
	} else {
		ratio=double(geoHeight)/double(image->page().height());
		imgGeo=Geometry(0, calcSize(image->rows(), ratio));
	}
	pageGeo=Geometry(calcSize(image->page().width(), ratio), calcSize(image->page().height(), ratio), 
			calcSize(image->page().xOff(), ratio), calcSize(image->page().yOff(), ratio));
}

void GeoCalculator::setAutoOption(Image *&image, Geometry &imgGeo, Geometry &pageGeo)
{
	double ratio=double(image->page().width())/double(image->page().height());
	Geometry preCropGeo;
	
	if(ratio > this->minRatio*2 && ratio < this->maxRatio/2) {	// f mode
		this->setFixedOption(image, imgGeo, pageGeo);
	} else if(ratio > this->minRatio && ratio < this->maxRatio) {	// m mode
		this->setMobileOption(image, imgGeo, pageGeo);
		this->setNoresize(image, imgGeo, pageGeo);
	} else if(image->page().height() > this->geoHeight || image->page().width() > this->geoWidth) {	// f+m mode
		if(ratio > this->maxRatio) {	// wide
			preCropGeo=Geometry(calcSize(geoHeight, this->tolerance), this->geoHeight);
		} else {	// long
			preCropGeo=Geometry(this->geoWidth, calcSize(geoWidth, this->tolerance));
		}
		//crop
		preCropGeo.xOff(int((double(image->page().width())-double(preCropGeo.width()))/2));
		preCropGeo.xOff(int((double(image->page().height())-double(preCropGeo.height()))/2));
		image->crop(preCropGeo);
		preCropGeo.xOff(image->page().xOff()-preCropGeo.xOff());
		preCropGeo.yOff(image->page().yOff()-preCropGeo.yOff());
		image->page(preCropGeo);

		this->setMobileOption(image, imgGeo, pageGeo);
		this->setNoresize(image, imgGeo, pageGeo);
	} else {	// Wide or Long but too Small to Crop
		this->setMobileOption(image, imgGeo, pageGeo);
		this->setNoresize(image, imgGeo, pageGeo);
	}
}

void GeoCalculator::setNoresize(Image *&image, Geometry &imgGeo, Geometry &pageGeo)
{
	if(imgGeo.width() > image->page().width() || imgGeo.height() > image->page().height()) {
		imgGeo=Geometry(0, 0, 0, 0);
		pageGeo=Geometry(0, 0, 0, 0);
	}
}
