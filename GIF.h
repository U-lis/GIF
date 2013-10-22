#ifndef _GIF_H
#define _GIF_H

#include "GIFLocal.h"
#include <list>
#include <exception>

using std::list;

static const GIFBlobData APP_EXT[14] = {0x21, 0xFF, 0x0B, 'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E', 2'', '.', '0'};
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

		void readGIF(const GIFBlobData *inputStream, const int fileSize);
		void readGIFHeader(GIFBlobData *&ptr, const GIFBlobData *endp);
		bool isGIF();
		void readLogicalScreen(GIFBlobData *&ptr, const GIFBlobData *endp);
		void readGCT(GIFBlobData *&ptr, const GIFBlobData *endp);
		void readExtension(GIFBlobData *&ptr, const GIFBlobData *endp);

	public:
		GIF(const char *filename);
		GIF(const GIFBlobData *inputStream, const int fileSize);
		GIF(list<GIF*> GIFList);
		~GIF();

		void writeBlob(GIFBlobData *outputStream);
		void writeFile(char* filename);
};

#endif	// _GIF_H
