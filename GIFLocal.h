#ifndef _GIF_LOCAL_H
#define _GIF_LOCAL_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using std::ofstream;

typedef unsigned char GIFBlobData;
enum CODE {TERM_CODE=0x00, EXTENSION_CODE=0x21, IMAGE_CODE=0x2C, APPE_CODE=0xFF, GCE_CODE=0xF9, TRAILER_CODE=0x3B};
enum SIZE {GIF_HEADER_SIZE=6, LOGICAL_SCREEN_SIZE=7, GIF_LOCAL_IMAGE_HEADER_SIZE=10, GCE_SIZE=8, APPE_SIZE=19};

class GIFLocalImage {
	private:
		GIFBlobData *GCE;
		GIFBlobData imageHeader[GIF_LOCAL_IMAGE_HEADER_SIZE];
		GIFBlobData *LCT;
		int LCTSize;
		GIFBlobData LZW;
		GIFBlobData *dataBlock;
		int dataBlockSize;

		int localImageSize;

		void readExtension(GIFBlobData *&ptr, const GIFBlobData *endp);
		void setGCTtoLCT(const GIFBlobData *GCT, const int GCTSize, const bool sorted);
		void readImage(const GIFBlobData *GCT, const int GCTSize, const bool sorted, GIFBlobData *&ptr, const GIFBlobData *endp);
		void readDataBlock(GIFBlobData *&ptr, const GIFBlobData *endp);
		const int getImageWidth();
		const int getImageHeight();
		const int getImageXOffset();
		const int getImageYOffset();
		void setImageOffset(const int xOff, const int yOff);


		void ignore(GIFBlobData *&ptr);
		bool safe(const GIFBlobData* ptr, const int size, const GIFBlobData *endp);

	public:
		GIFLocalImage(const GIFBlobData *GCT, const int GCTSize, const bool sorted, GIFBlobData *&ptr, const GIFBlobData *endp);
		~GIFLocalImage();

		const int getLocalImageSize(){return this->localImageSize;}
		void setMaxScreenOffset(const int maxW, const int maxH);

		void writeImageBlob(GIFBlobData *&output);
		void writeImageFile(ofstream &output);
};
#endif	// _GIF_LOCAL_H
