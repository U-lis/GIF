#include "GIF.h"
#include <fstream>

using std::exception;
using std::ifstream;
using std::ios_base;
using std::ios;
using std::cout;
using std::endl;

bool GIF::safe(GIFBlobData *ptr, int size, const GIFBlobData *endp)
{
	if(ptr+size<endp)
		return true;
	return false;
}

void GIF::ignore(GIFBlobData *&ptr)
{
	ptr++;
	while(*ptr!=TERM_CODE) {
		ptr+=(*ptr)+1;
	}
	ptr++;
}

GIF::GIF(const char *filename)
	:GCT(NULL), GCTSize(0), sorted(false), appE(false), imgCnt(0), GIFSize(0)
{
	try {
		ifstream inputFile(filename, ios_base::binary);
		if(!inputFile) {
			cout << "Input File Open Failed" << endl;
			exit(EXIT_FAILURE);
		}
		
		inputFile.seekg(0, ios::end);
		int fileSize=inputFile.tellg();
		inputFile.seekg(0, ios::beg);

		GIFBlobData *inputStream=new GIFBlobData[fileSize];
		inputFile.read((char*)inputStream, fileSize);
		readGIF(inputStream, fileSize);
		delete[] inputStream;		
	}
	catch(const exception &e) {
		cout << "Exception : " << e.what() << endl;		
	}	
}

GIF::GIF(GIFBlobData *inputStream, const int fileSize)
	:GCT(NULL), GCTSize(0), sorted(false), appE(false), imgCnt(0), GIFSize(0)
{
	readGIF(inputStream, fileSize);
}

GIF::GIF(const list<GIF*> &GIFList)
	:GCT(NULL), GCTSize(0), sorted(false), appE(false), imgCnt(0), GIFSize(0)
{
	list<GIF*>::const_iterator itr = GIFList.begin();
	memcpy(GIFHeader, (*itr)->getGIFHeader(), GIF_HEADER_SIZE);
	memcpy(logicalScreen, (*itr)->getLogicalScreen(), LOGICAL_SCREEN_SIZE);
	logicalScreen[4]&=0x70;
	appE=true;
	
	this->setMaxLogicalScreen(GIFList);
	
	for(itr=GIFList.begin(); itr!=GIFList.end(); itr++) {
		this->localImageList.assign((*itr)->getLocalImageList().begin(), (*itr)->getLocalImageList().end());
	}

	this->imgCnt = this->localImageList.size();

	this->setLocalImageScreen();
}

GIF::~GIF()
{
	if(localImageList.size()!=0) {
		for(list<GIFLocalImage*>::iterator itr=localImageList.begin(); itr!=localImageList.end(); itr++) {
			delete *itr;
		}
	}
}

void GIF::readGIF(GIFBlobData *inputStream, const int fileSize)
{
	GIFBlobData *ptr = inputStream;
	const GIFBlobData *endp=ptr+fileSize;
	this->readGIFHeader(ptr, endp);
	this->readLogicalScreen(ptr, endp);
	this->readGCT(ptr, endp);
	while(*ptr!=TRAILER_CODE) {
		switch(*ptr) {
			case EXTENSION_CODE:
				this->readExtension(ptr, endp);
				break;
			case IMAGE_CODE:
				this->localImageList.push_back(new GIFLocalImage(this->GCT, this->GCTSize, this->sorted, ptr, endp));
				break;
			default:
				cout << "default CODE. Ignore it." << endl;
				this->ignore(ptr);
				break;
		}
	}

	if(GCT != NULL) {
		delete[] this->GCT;
		this->GCT = NULL;
	}
	this->GCTSize = 0;
	this->imgCnt = this->localImageList.size();
	cout << "GIF File Read Done" << endl;
}

void GIF::readGIFHeader(GIFBlobData *&ptr, const GIFBlobData *endp)
{
	if(this->safe(ptr, GIF_HEADER_SIZE, endp)) {
		memcpy(this->GIFHeader, ptr, GIF_HEADER_SIZE);
		ptr+=GIF_HEADER_SIZE;
	}
	else {
		cout << "safe fail : GIF Header" << endl;
		exit(EXIT_FAILURE);
	}

	if(!this->isGIF()) {
		cout << "Abnormal GIF Signature Error" << endl;
		exit(EXIT_FAILURE);
	}
}

bool GIF::isGIF()
{
	if(!(this->GIFHeader[0] == 'G' && this->GIFHeader[1] == 'I' && this->GIFHeader[2] == 'F')) {
		return false;
	}
	if(!(this->GIFHeader[3] == '8' && (this->GIFHeader[4] == '9' || this->GIFHeader[4] == '7') && this->GIFHeader[5] == 'a')) {
		return false;
	}
	if(this->GIFHeader[4] == '7')
		this->GIFHeader[4]='9';
	return true;
}

void GIF::readLogicalScreen(GIFBlobData *&ptr, const GIFBlobData *endp)
{
	if(safe(ptr, LOGICAL_SCREEN_SIZE, endp)) {
		memcpy(this->logicalScreen, ptr, LOGICAL_SCREEN_SIZE);
		ptr+=LOGICAL_SCREEN_SIZE;
	}
	else {
		cout << "safe fail : Logical Screen" << endl;
		exit(EXIT_FAILURE);
	}
}

void GIF::readGCT(GIFBlobData *&ptr, const GIFBlobData *endp)
{
	if(this->logicalScreen[4]&0x80 != 0x80) {
		// cout << "NO GCT" << endl;
	}
	else {
		this->GCTSize=3*(2<<(this->logicalScreen[4]&0x07));
		this->GCT=new GIFBlobData[this->GCTSize];
		if(safe(ptr, this->GCTSize, endp)) {
			memcpy(this->GCT, ptr, this->GCTSize);
			ptr+=this->GCTSize;
		}
		else {
			cout << "safe fail : GCT" << endl;
			exit(EXIT_FAILURE);
		}
	}

	if(this->logicalScreen[4]&0x08 == 0x08) {
		this->sorted=true;
	}
}

void GIF::readExtension(GIFBlobData *&ptr, const GIFBlobData *endp)
{
	ptr++;
	switch(*ptr) {
		case APPE_CODE:
			ptr++;
			while(*ptr!=TERM_CODE) {
				GIFBlobData *appe=new GIFBlobData[*ptr];
				if(safe(ptr+1, *ptr, endp)) {
					memcpy(appe, ptr+1, *ptr);
					ptr+=(*ptr)+1;
					if(strcmp((char*)appe, "NETSCAPE2.0")) {
						this->appE=true;
					}
				}
				else {
					cout << "safe fail : Application Extension" << endl;
					exit(EXIT_FAILURE);
				}
			}
			break;
		case GCE_CODE:
			ptr--;
			this->localImageList.push_back(new GIFLocalImage(this->GCT, this->GCTSize, this->sorted, ptr, endp));
			break;
	}
}

void GIF::writeBlob(GIFBlobData *outputStream, const int imgNum)
{
	GIFBlobData *ptr = outputStream;

	memcpy(ptr, this->GIFHeader, GIF_HEADER_SIZE);
	ptr += GIF_HEADER_SIZE;
	memcpy(ptr, this->logicalScreen, LOGICAL_SCREEN_SIZE);
	ptr += LOGICAL_SCREEN_SIZE;

	if(this->appE || imgNum == ALLIMGS) {
		memcpy(ptr, APP_EXT, APPE_SIZE);
		ptr += APPE_SIZE;
	}

	list<GIFLocalImage*>::iterator imgItr=this->localImageList.begin();
	
	if(imgNum == ALLIMGS) {
		for(; imgItr != this->localImageList.end(); imgItr++) {
			(*imgItr)->writeImageBlob(ptr);
		}
	} else {
		for(int i=0; i<imgNum; i++)
			imgItr++;
		(*imgItr)->writeImageBlob(ptr);
	}
	*ptr = TRAILER_CODE;
}

void GIF::writeFile(const char* filename, const int imgNum)
{
	ofstream output(filename, ios_base::binary);
	if(!output) {
		cout << "Writing File Open Failed" << endl;
		return;
	}

	output.write((char*)this->GIFHeader, GIF_HEADER_SIZE);
	output.write((char*)this->logicalScreen, LOGICAL_SCREEN_SIZE);

	if(this->appE || imgNum == ALLIMGS) {
		output.write((char*)APP_EXT, APPE_SIZE);
	}

	list<GIFLocalImage*>::iterator imgItr = this->localImageList.begin();

	if(imgNum == ALLIMGS) {
		for(; imgItr != this->localImageList.end(); imgItr++)
			(*imgItr)->writeImageFile(output);
	} else {
		for(int i=0; i<imgNum; i++)
			imgItr++;
		(*imgItr)->writeImageFile(output);
	}
	output.put(TRAILER_CODE);
	output.close();
}

void GIF::setMaxLogicalScreen(const list<GIF*> &GIFList)
{
	int maxW = 0, maxH = 0;

	list<GIF*>::const_iterator itr = GIFList.begin();
	for(; itr != GIFList.end(); itr++) {
		if((*itr)->getLogicalWidth() > maxW) {
			maxW = (*itr)->getLogicalWidth();
		}

		if((*itr)->getLogicalHeight() > maxH) {
			maxH = (*itr)->getLogicalHeight();
		}
	}

	this->logicalScreen[0] = maxW    & 0xFF;
	this->logicalScreen[1] = maxW>>8 & 0xFF;
	this->logicalScreen[2] = maxH    & 0xFF;
	this->logicalScreen[3] = maxH>>8 & 0xFF;
}

const int GIF::getLogicalWidth() {
	return this->logicalScreen[0] | this->logicalScreen[1]<<8;
}

const int GIF::getLogicalHeight() {
	return this->logicalScreen[2] | this->logicalScreen[3]<<8;
}

void GIF::setLocalImageScreen() 
{
	list<GIFLocalImage*>::iterator itr = this->localImageList.begin();

	for(; itr != this->localImageList.end(); itr++) {
		(*itr)->setMaxScreenOffset(this->getLogicalWidth(), this->getLogicalHeight());
	}
}

const int GIF::getGIFSize(const int imgNum)
{	
	int size=this->GIFSize;
	
	list<GIFLocalImage*>::const_iterator imgItr = this->localImageList.begin();

	if(imgNum == ALLIMGS) {
		for(; imgItr != this->localImageList.end(); imgItr++)
			size += (*imgItr)->getLocalImageSize();
	} else {
		for(int i=0; i<imgNum; i++)
			imgItr++;
		size += (*imgItr)->getLocalImageSize();
	}
}
