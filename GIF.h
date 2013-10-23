#ifndef _GIF_H
#define _GIF_H

#include "GIFLocal.h"
#include <list>
#include <exception>

using std::list;

static const GIFBlobData APP_EXT[APPE_SIZE] = {0x21, 0xFF, 0x0B, 'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E', '2', '.', '0', 0x03, 0x01, 0x00, 0x00, 0x00};
static const int ALLIMGS = -1;

class GIF {
	private:
		GIFBlobData GIFHeader[GIF_HEADER_SIZE];
		GIFBlobData logicalScreen[LOGICAL_SCREEN_SIZE];
		bool appE;
		GIFBlobData *GCT;
		int GCTSize;
		bool sorted;
		list<GIFLocalImage*> localImageList;
		int imgCnt;

		int GIFSize;

		bool safe(GIFBlobData *ptr, int size, const GIFBlobData *endp);
		void ignore(GIFBlobData *&ptr);

		void readGIF(GIFBlobData *inputStream, const int fileSize);
		void readGIFHeader(GIFBlobData *&ptr, const GIFBlobData *endp);
		bool isGIF();
		void readLogicalScreen(GIFBlobData *&ptr, const GIFBlobData *endp);
		void readGCT(GIFBlobData *&ptr, const GIFBlobData *endp);
		void readExtension(GIFBlobData *&ptr, const GIFBlobData *endp);
		void setMaxLogicalScreen(const list<GIF*> &GIFList);
		const int getLogicalWidth();
		const int getLogicalHeight();
		void setLocalImageScreen();

	public:
		GIF(const char *filename);
		GIF(GIFBlobData *inputStream, const int fileSize);
		GIF(const list<GIF*> &GIFList);
		~GIF();

		const GIFBlobData* getGIFHeader(){return this->GIFHeader;}
		const GIFBlobData* getLogicalScreen(){return this->logicalScreen;}
		const bool getappE(){return this->appE;}
		const GIFBlobData* getGCT(){return this->GCT;}
		const int getGCTSize(){return this->GCTSize;}
		const bool getSorted(){return this->sorted;}
		const int getImgCnt(){return this->imgCnt;}
		const int getGIFSize(const int imgNum);
		const list<GIFLocalImage*>& getLocalImageList() {return this->localImageList;}

		void writeBlob(GIFBlobData *outputStream, const int imgNum);
		void writeFile(const char* filename, const int imgNum);
};

#endif	// _GIF_H
