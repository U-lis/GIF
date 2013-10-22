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

GIF::GIF(const GIFBlobData *inputStream, const int fileSize)
	:GCT(NULL), GCTSize(0), sorted(false), appE(false), imgCnt(0), GIFSize(0)
{
	readGIF(inputStream, fileSize);
}

GIF::GIF(list<GIF*> GIFList)
	:GCT(NULL), GCTSize(0), sorted(false), appE(false), imgCnt(0), GIFSize(0)
{
	//TODO
}

GIF::~GIF()
{
	if(localImageList.size()!=0) {
		for(list<GIFLocalImage*>::iterator itr=localImageList.begin(); itr!=localImageList.end(); itr++) {
			delete *itr;
		}
	}
	if(this->GCT!=NULL) {
		delete[] GCT;
		GCT=NULL;
	}
}

void GIF::readGIF(const GIFBlobData *inputStream, const int fileSize)
{
	GIFBlobData *ptr=NULL;
	*ptr=*inputStream;
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
	if(!(this->GIFHeader[3] == '8' && (this->GIFHeader[5] == '9' || this->GIFHeader[4] == '7') && this->GIFHeader[5] == 'a')) {
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

void writeBlob(GIFBlobData *outputStream)
{
	//TODO
}

void writeFile(char* filename)
{
	//TODO
}

