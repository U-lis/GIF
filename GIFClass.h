#ifndef _GIF_V2_H_
#define _GIF_V2_H_

#include <fstream>
#include <string>
#include <exception>

#include "GIFLocalImage.h"

using std::list;
using std::string;
using std::exception;
using std::endl;
using std::cout;
using std::cin;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::ios_base;

static const GIFData APP_EXT[14]={0x21, 0xFF, 0x0B, 'N','E','T','S','C','A','P','E','2','.','0'};
static const int ALLIMGS=-1;

class GIF{
	private:
		GIFData GIFHeader[SIZE_GIF_HEADER];
		GIFData logicalScreen[SIZE_LOGICAL_SCREEN];
		GIFData* GCT;
		int GCTSize;
		bool sort;
		bool appExt;
		short aniItr;
		list<GIFImage*> imageList;
		int imageCnt;
		int GIFSize;
		
		void getGIF(GIFData* ptr, int fileSize);

	public:
		GIF(GIFData* stream, int fileSize);
		GIF(char* filename);
		GIF(list<GIF*> gifList);
		~GIF();

		const GIFData* getGIFHeader(){return GIFHeader;}
		const GIFData* getLogicalScreen(){return logicalScreen;}
		const GIFData* getGCT(){return GCT;}
		const int getGCTSize(){return GCTSize;}
		const bool getAppExt(){return appExt;}
		const int getImageCnt(){return imageCnt;}
		short getAniItr(){return aniItr;}
		list<GIFImage*> & getImageList(){return imageList;}
		const int getGIFSize(int i);
		
		void writeBlob(GIFData* outputStream, int num);	// set num to ALLIMGS(static const int -1) to Write All Images
		void writeFile(char* filename, int num);

};

#endif	// ifndef _GIF_V2_H_
