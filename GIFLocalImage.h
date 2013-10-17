#ifndef _GIF_IMAGE_H_
#define _GIF_IMAGE_H_

#include <iostream>
#include <list>
#include <cstring>

using std::ofstream;

typedef unsigned char GIFData;
enum CODE {EXTENSION=0x21, IMAGE=0x2C, APPE=0xFF, GCE=0xF9, TERM=0x00, TRAILER=0x3B};
enum SIZE {SIZE_GIF_HEADER=6, SIZE_LOGICAL_SCREEN=7, SIZE_GIF_IMAGE_HEADER=10, SIZE_GCE=8, SIZE_APPE=19};

class GIFLocalImage{
	private:
		GIFData GCE[SIZE_GCE];
		GIFData imageHeader[SIZE_GIF_IMAGE_HEADER];
		GIFData* LCT;
		int LCTSize;
		GIFData LZW;
		GIFData* dataBlock;
		int dataBlockSize;

		int imageSize;

	public:
		//GIFLocalImage(GIFLocalImage & image);
		GIFLocalImage(GIFData* GCT, int GCTSize, bool sort, GIFData** ptr, GIFData* endp);
		~GIFLocalImage();

		GIFData* getGCE() {return GCE;}
		GIFData* getImageHeader(){return imageHeader;}

		GIFData* getLCT() {return LCT;}
		const int getLCTSize(){return LCTSize;}

		const GIFData getLZW(){return LZW;}
		void setLZW(GIFData data){LZW=data;}

		GIFData* getDataBlock(){return dataBlock;}
		const int getDataBlockSize(){return dataBlockSize;}

		const int getImageSize(){return imageSize;}

		GIFData* writeImageBlob(GIFData* outputStream);
		void writeImageFile(ofstream & out);
};

#endif	//ifndef _GIF_IMAGE_H
