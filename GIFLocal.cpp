#include "GIFLocal.h"

using std::cout;
using std::endl;

bool GIFLocalImage::safe(const GIFBlobData *ptr, const int size, const GIFBlobData *endp)
{
	if(ptr+size<endp)
		return true;
	return false;
}

void GIFLocalImage::ignore(GIFBlobData *&ptr)
{
	ptr++;
	while(*ptr!=TERM_CODE) {
		ptr+=(*ptr)+1;
	}
	ptr++;
}

GIFLocalImage::GIFLocalImage(const GIFBlobData *GCT, const int GCTSize, const bool sorted, GIFBlobData *&ptr, const GIFBlobData *endp) 
: GCE(NULL), LCT(NULL), LCTSize(0), dataBlock(NULL), dataBlockSize(0), localImageSize(0)
{
	switch(*ptr)
	{
		case EXTENSION_CODE : 
			readExtension(ptr, endp);
			break;
		case IMAGE_CODE:
			readImage(GCT, GCTSize, sorted, ptr, endp);
			break;
		default:
			cout << "default Image Part. Ignore it." << endl;
			ignore(ptr);
			break;
	}
}

GIFLocalImage::~GIFLocalImage()
{
	if(GCE!=NULL) {
		delete[] GCE;
		GCE=NULL;
	}
	if(LCT!=NULL) {
		delete[] LCT;
		LCT=NULL;
	}
	if(dataBlock!=NULL) {
		delete[] dataBlock;
		dataBlock=NULL;
	}
}

void GIFLocalImage::readExtension(GIFBlobData *&ptr, const GIFBlobData *endp)
{
	ptr++;
	switch(*ptr) {
		case GCE_CODE : 
			ptr++;
			if(safe(ptr+1, *ptr, endp)) {
				this->GCE=new GIFBlobData[GCE_SIZE];
				memcpy(this->GCE, ptr-2, GCE_SIZE);
				ptr+=(*ptr)+2;
			}
			break;
		default:
			cout << "default Extension. Ignore it." << endl;
			this->ignore(ptr);
			break;
	}
}

void GIFLocalImage::readImage(const GIFBlobData *GCT, const int GCTSize, const bool sorted, GIFBlobData *&ptr, const GIFBlobData *endp)
{
	if(this->safe(ptr+1, GIF_LOCAL_IMAGE_HEADER_SIZE, endp)) {
		memcpy(this->imageHeader, ptr, GIF_LOCAL_IMAGE_HEADER_SIZE);
		ptr+=GIF_LOCAL_IMAGE_HEADER_SIZE;
	}
	else {
		cout << "safe fail : imageHeader" << endl;
		exit(EXIT_FAILURE);
	}

	if((this->imageHeader[9]&0x80)==0x80) {	// LCT Exist
		this->LCTSize=(3*(2<<(this->imageHeader[9]&0x07)));
		if(safe(ptr, this->LCTSize, endp)) {
			this->LCT=new GIFBlobData[LCTSize];
			memcpy(this->LCT, ptr, this->LCTSize);
			ptr+=this->LCTSize;
		}
		else {
			cout << "safe fail : LCT" << endl;
			exit(EXIT_FAILURE);
		}
	}
	else {
		this->setGCTtoLCT(GCT, GCTSize, sorted);
	}
	this->readDataBlock(ptr, endp);
	ptr++;	// Indicates Next Something's Start
	this->localImageSize=(this->GCE==NULL?0:GCE_SIZE) + GIF_LOCAL_IMAGE_HEADER_SIZE + this->LCTSize + 1 + this->dataBlockSize;
}

void GIFLocalImage::setGCTtoLCT(const GIFBlobData *GCT, const int GCTSize, const bool sorted)
{
	this->imageHeader[9]|=0x80;
	if(sorted) {
		this->imageHeader[9]|=0x20;
	}
	this->LCTSize=GCTSize;
	int GSize=GCTSize/3;
	int cnt=0;
	while(GSize>2) {
		cnt++;
		GSize/=2;
	}
	imageHeader[9]|=cnt;
	this->LCT=new GIFBlobData[this->LCTSize];
	memcpy(this->LCT, GCT, this->LCTSize);
}

void GIFLocalImage::readDataBlock(GIFBlobData *&ptr, const GIFBlobData *endp)
{
	this->LZW=*ptr;
	ptr++;
	
	int dataSize=0;
	GIFBlobData *start=NULL;

	start=ptr;
	while(*ptr!=TERM_CODE) {
		if(safe(ptr+1, *ptr, endp)) {
			dataSize+=(*ptr)+1;
			ptr+=(*ptr)+1;
		}
		else {
			cout << "safe fail : dataBlock" << endl;
			exit(EXIT_FAILURE);
		}
	}
	dataSize++;
	this->dataBlockSize=dataSize;
	this->dataBlock=new GIFBlobData[this->dataBlockSize];
	memcpy(this->dataBlock, start, this->dataBlockSize);
}

void GIFLocalImage::writeImageBlob(GIFBlobData *&output)
{
	if(this->GCE == NULL) {
		//cout << "NOGCE" << endl;
	} else {
		memcpy(output, this->GCE, GCE_SIZE);
		output += GCE_SIZE;
	}

	memcpy(output, this->imageHeader, GIF_LOCAL_IMAGE_HEADER_SIZE);
	output += GIF_LOCAL_IMAGE_HEADER_SIZE;

	if(this->LCT == NULL) {
		//cout << "NOLCT" << endl;
	} else {
		memcpy(output, this->LCT, this->LCTSize);
		output += this->LCTSize;
	}

	*output=this->LZW;
	output++;

	memcpy(output, this->dataBlock, this->dataBlockSize);
	output += this->dataBlockSize;
}

void GIFLocalImage::writeImageFile(ofstream &output) 
{
	if(this->GCE == NULL) {
		//cout << "NOGCE" << endl;
	} else {
		output.write((char*)this->GCE, GCE_SIZE);
	}

	output.write((char*)this->imageHeader, GIF_LOCAL_IMAGE_HEADER_SIZE);

	if(this->LCT == NULL) {
		//cout << "NOLCT" << endl;
	} else {
		output.write((char*)this->LCT, this->LCTSize);
	}
	output.put((char)this->LZW);
	output.write((char*)this->dataBlock, this->dataBlockSize);
}

void GIFLocalImage::setMaxScreenOffset(const int maxW, const int maxH)
{
	int xOff=((maxW-this->getImageWidth())/2)+this->getImageXOffset();
	int yOff=((maxH-this->getImageHeight())/2)+this->getImageYOffset();

	this->setImageOffset(xOff, yOff);
}

const int GIFLocalImage::getImageWidth()
{
	return this->imageHeader[5] | this->imageHeader[6]<<8;
}

const int GIFLocalImage::getImageHeight()
{
	return this->imageHeader[7] | this->imageHeader[8]<<8;
}

const int GIFLocalImage::getImageXOffset()
{
	return this->imageHeader[1] | this->imageHeader[2]<<8;
}

const int GIFLocalImage::getImageYOffset()
{
	return this->imageHeader[3] | this->imageHeader[4]<<8;
}

void GIFLocalImage::setImageOffset(const int xOff, const int yOff)
{
	this->imageHeader[1] = xOff    & 0xFF;
	this->imageHeader[2] = xOff>>8 & 0xFF;
	this->imageHeader[3] = yOff    & 0xFF;
	this->imageHeader[4] = yOff>>8 & 0xFF;
}
