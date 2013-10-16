#include "GIFClass.h"
#include <stdio.h>

using std::ios;
using std::ios_base;

bool safe(GIFData* ptr, int size, GIFData* endp)
{
	if(ptr+size<endp)
		return true;
	return false;
}

GIF::GIF(char* filename):GCT(NULL), GCTSize(0), appExt(false), aniItr(0), imageCnt(0), sort(false)
{
	try{
		ifstream inputFile(filename, ios_base::binary);
		if(!inputFile){
			cout << "Input File Open Failed" << endl;
			exit(EXIT_FAILURE);
		}
		inputFile.seekg(0, ios::end);
		int fileSize=inputFile.tellg();
		inputFile.seekg(0, ios::beg);

		GIFData* inputStream=new GIFData[fileSize];
		inputFile.read((char*)inputStream, fileSize);
		getGIF(inputStream, fileSize);
		delete[] inputStream;
	}
	catch(exception e){
		cout << endl << "exception!!" << endl;
		cout << e.what() << endl;
	}
}

GIF::GIF(GIFData* ptr, int fileSize):GCT(NULL), GCTSize(0), appExt(false), aniItr(0), imageCnt(0), sort(false)
{
	getGIF(ptr, fileSize);
}

GIF::GIF(list<GIF*> gifList):GCT(NULL), GCTSize(0), appExt(false), aniItr(0), imageCnt(0), sort(false)
{
	list<GIF*>::iterator gifItr=gifList.begin();
	memcpy(GIFHeader, (*gifItr)->getGIFHeader(), SIZE_GIF_HEADER);
	memcpy(logicalScreen, (*gifItr)->getLogicalScreen(), SIZE_LOGICAL_SCREEN);
	logicalScreen[4]&=0x70;
	appExt=true;

	int maxW=0, maxH=0;
	for(; gifItr!=gifList.end(); gifItr++)
	{
		//cout << "w: " << int((*((*gifItr)->getLogicalScreen()+1))<<8) << " " << int(*((*gifItr)->getLogicalScreen())) << endl;
		if((((*((*gifItr)->getLogicalScreen()+1))<<8)|((*((*gifItr)->getLogicalScreen()))))>maxW)
			maxW=(((*((*gifItr)->getLogicalScreen()+1))<<8)|((*((*gifItr)->getLogicalScreen()))));
		
		if((((*((*gifItr)->getLogicalScreen()+3))<<8)|((*((*gifItr)->getLogicalScreen()+2))))>maxH)
			maxH=(((*((*gifItr)->getLogicalScreen()+3))<<8)|((*((*gifItr)->getLogicalScreen()+2))));

		list<GIFLocalImage*>::iterator imageItr=imageList.begin();
		for(int i=0; i<imageCnt; i++, imageItr++);

		imageList.splice(imageItr, (*gifItr)->getImageList());
		//cout << "splice : " << imageList.size() << endl;
		imageCnt=imageList.size();
	}	
	
	cout.setf(ios::dec);
	cout << "MAX : " << maxW << " " << maxH << endl;
	logicalScreen[0]=maxW&0xFF;
	logicalScreen[1]=maxW>>8&0xFF;
	logicalScreen[2]=maxH&0xFF;
	logicalScreen[3]=maxH>>8&0xFF;

	for(list<GIFLocalImage*>::iterator imageItr=imageList.begin(); imageItr!=imageList.end(); imageItr++)
	{
	int width=0, height=0, xOffset=0, yOffset=0, xOff=0, yOff=0;
		width=(((*((*imageItr)->getImageHeader()+6))<<8)|(*((*imageItr)->getImageHeader()+5)));
		height=(((*((*imageItr)->getImageHeader()+8))<<8)|(*((*imageItr)->getImageHeader()+7)));
		cout << "width : " << width << " height : " << height << endl;
		xOffset=(maxW-width)/2;
		yOffset=(maxH-height)/2;
		cout << "xOffset : " << xOffset << " yOffset : " << yOffset << endl;
		xOff=((*((*imageItr)->getImageHeader()+2))<<8) | (*((*imageItr)->getImageHeader()+1));
		yOff=((*((*imageItr)->getImageHeader()+4))<<8) | (*((*imageItr)->getImageHeader()+3));
		cout << "xOff : " << xOff << " y : " << yOff << endl;
		*((*imageItr)->getImageHeader()+2)=(xOff+xOffset)>>8&0xFF;
		*((*imageItr)->getImageHeader()+1)=(xOff+xOffset)&0xFF;
		*((*imageItr)->getImageHeader()+4)=(yOff+yOffset)>>8&0xFF;
		*((*imageItr)->getImageHeader()+3)=(yOff+yOffset)&0xFF;
	}

}

GIF::~GIF()
{
	if(imageList.size()!=0)
	{
		list<GIFLocalImage*>::iterator itr=imageList.begin();
		for(; itr!=imageList.end(); itr++)
		{
			/*if(this->GCT==(*itr)->getLCT())
			{
				cout << "same" << endl;
				(*itr)->resetLCT();
			}*/
			delete *itr;
		}
		imageList.clear();
	}
	if(GCT!=NULL)
	{
		delete[] GCT;
		GCT=NULL;
	}
}

void GIF::getGIF(GIFData* ptr, int fileSize)
{
	//cout << "getGIF" << endl;///////////////
	GIFData* endp=ptr+fileSize;
	try{

		if(safe(ptr, SIZE_GIF_HEADER, endp)){
			memcpy(this->GIFHeader, ptr, SIZE_GIF_HEADER);
			ptr+=SIZE_GIF_HEADER;
		}
		else{
			cout << "safe fail : GIFHeader" << endl;
			exit(EXIT_FAILURE);
		}
		if(!(this->GIFHeader[0]=='G' && this->GIFHeader[1]=='I' && this->GIFHeader[2]=='F')){
			cout << "NOT GIF" << endl;
			exit(EXIT_FAILURE);
		}
		if(!(this->GIFHeader[3]=='8' && (this->GIFHeader[4]=='7' || this->GIFHeader[4]=='9') && this->GIFHeader[5]=='a')){
			cout << "NOT 87a or 89a" << endl;
			exit(EXIT_FAILURE);
		}
		if(this->GIFHeader[4]=='7')
			this->GIFHeader[4]='9';
		if(safe(ptr, SIZE_LOGICAL_SCREEN, endp)){
			memcpy(this->logicalScreen, ptr, SIZE_LOGICAL_SCREEN);
			ptr+=SIZE_LOGICAL_SCREEN;
		}
		else{
			cout << "safe fail : logicalScreen" << endl;
			exit(EXIT_FAILURE);
		}
		//cout << "logicalScreen : " << (int(this->logicalScreen[4])&0x80) << endl;
		if((this->logicalScreen[4]&0x80)==0x80){
			this->GCTSize=3*(2<<(logicalScreen[4]&0x07));
			//cout << "GCT : " << this->GCTSize << " " << int(*(ptr+3)) << endl;///////////////
			this->GCT=new GIFData[this->GCTSize];
			if(safe(ptr, this->GCTSize, endp)){
				memcpy(this->GCT, ptr, this->GCTSize);
				ptr+=GCTSize;
			}
			else{
				cout << "safe fail : GCT" << endl;
				exit(EXIT_FAILURE);
			}
		}
		else
			cout << "NOGCT" << endl;
		if(this->logicalScreen[4]&0x08==0x08)
			this->sort=true;
		bool exitFlag=false;
		while(*ptr!=TRAILER)
		{
			//cout << "ptr  " << int(*ptr) << "  " << int(*(ptr+1)) << endl;
			switch(*ptr)
			{
				case EXTENSION:
					//cout << "extension" << endl;/////////////
					ptr++;
					switch(*ptr)
					{
						case APPE:
							cout << "appe" << endl;
							ptr++;
							while(*ptr!=TERM){
								GIFData* appe=new GIFData[*ptr];
								if(safe(ptr+1, *ptr, endp)){
									memcpy(appe, ptr+1, *ptr);
									ptr+=*ptr+1;
									if(strcmp((char*)appe, "NETSCAPE2.0")==0){
										appExt=true;
										aniItr=*(ptr+3)<<2 | *(ptr+2);
										ptr+=*ptr+1;
									}
								}
								else{
									cout << "safe fail : APPE" << endl;
									exit(EXIT_FAILURE);
								}
								delete[] appe;
							}
							ptr++;
							cout << "itr : " << aniItr << endl;
							break;
						case GCE:
							//cout << "GCE" << endl;
							ptr--;
							imageList.push_back(new GIFLocalImage(this->GCT, this->GCTSize, this->sort, &ptr, endp));
							//cout << "read " << int(*ptr) << "  " << imageList.size() << endl;
							break;
						default:
							cout << "Default Ext. : " << *ptr << endl;
							printf("%02x\n", *ptr);
							ptr++;
							while(*ptr!=TERM){
								printf("%02x\n", *ptr);
								ptr+=(*ptr+1);
							}
							ptr++;
							break;
					}
					break;
				case IMAGE:
					//cout << "image" << endl;
					imageList.push_back(new GIFLocalImage(this->GCT, this->GCTSize, this->sort, &ptr, endp));
					break;
				case TRAILER:
					//cout << "trailer" << endl;
					exitFlag=true;
					break;
				default:
					cout << "Default : " << int(*ptr) << endl;
					exitFlag=true;
					break;
			}
		}
		imageCnt=imageList.size();
		this->GCTSize=0;
		delete[] this->GCT;
		this->GCT=NULL;
		this->logicalScreen[4]&=0x70;
		this->sort=false;

		this->GIFSize=SIZE_GIF_HEADER+SIZE_LOGICAL_SCREEN+this->GCTSize+(appExt?SIZE_APPE:0)+1;
	}
	catch(exception e)
	{
		cout << endl << "exception!!" << endl;
		cout << e.what() << endl;
	}
}

const int GIF::getGIFSize(const int num)
{
	int size=this->gifSize;

	list<GIFLocalImage*>::iterator localItr=this->imageList.begin();
	if(num==ALLIMGS) {
		for(; localItr!=imageList.end(); localItr++) {
			size+=(*localItr)->getImageSize();
		}
	} else {
		for(int i=0; i<num; i++, localItr++);
		size+=(*localItr)->getImageSize();
	}
	return size;
}

void GIF::writeBlob(GIFData* outputStream, int num)
{
	GIFData* ptr=outputStream;
	//cout << "writeBlob" << endl;
	
	memcpy(ptr, this->GIFHeader, SIZE_GIF_HEADER);
	ptr+=SIZE_GIF_HEADER;
	memcpy(ptr, this->logicalScreen, SIZE_LOGICAL_SCREEN);
	ptr+=SIZE_LOGICAL_SCREEN;

	if(num!=ALLIMGS && this->GCTSize!=0)
	{
		//cout << "GCT : " << this->GCTSize << endl;
		memcpy(ptr, this->GCT, this->GCTSize);
		ptr+=this->GCTSize;
	}

	if(appExt || num==ALLIMGS)
	{
		cout << "appEXT" << endl;
		memcpy(ptr, APP_EXT, 14);	ptr+=14;
		*ptr++=0x03;	*ptr++=0x01;
		*ptr++=(this->aniItr>>2)&0xFF;
		*ptr++=this->aniItr&0xFF;
		*ptr++=TERM;
	}

	list<GIFLocalImage*>::iterator itr=this->imageList.begin();
	if(num==ALLIMGS)
	{
		//cout << "ALL" << endl;
		for(; itr!=this->imageList.end(); itr++)
			ptr=(*itr)->writeImageBlob(ptr);
	}
	else
	{
		//cout << "one" << endl;
		for(int c=0; c<num; c++, itr++);
		ptr=(*itr)->writeImageBlob(ptr);
	}
	*ptr=TRAILER;
	//cout << "write size : " << ptr-outputStream << endl;
}

void GIF::writeFile(char* filename, int num)
{
	ofstream output(filename, ios::binary);
	if(!output)
	{
		cout << "Writing File Open Failed" << endl;
		//exit(EXIT_FAILURE);
		return;
	}

	output.write((char*)this->GIFHeader, SIZE_GIF_HEADER);
	output.write((char*)this->logicalScreen, SIZE_LOGICAL_SCREEN);
	
	if(num!=-1 && this->GCTSize!=0)
		output.write((char*)this->GCT, this->GCTSize);
	
	if(appExt || num==ALLIMGS)
	{
		cout << "write appe" << endl;
		output.write((char*)&APP_EXT[0], 14);
		output.put(0x03).put(0x01);
		output.put((this->getAniItr()>>2)&0xFF);
		output.put((this->getAniItr()&0xFF));
		output.put(TERM);
	}

	list<GIFLocalImage*>::iterator itr=imageList.begin();

	if(num==ALLIMGS)
	{
		//cout << "all" << endl;
		for(; itr!=imageList.end(); itr++)
			(*itr)->writeImageFile(output);
	}
	else
	{
		for(int c=0; c<num; c++, itr++);
		(*itr)->writeImageFile(output);
	}
	output.put(TRAILER);
	//cout << "writeImage Done" << endl;
	output.close();
}

GIFData* GIFLocalImage::writeImageBlob(GIFData* ptr)
{
	//cout << "writeImageBlob" << endl;
	if(this->GCE!='\0')
	{
		//cout << "write gce" << endl;
		memcpy(ptr, this->GCE, SIZE_GCE);
		ptr+=SIZE_GCE;
	}
	else
	{
		//cout << "nowrite gce" << endl;
	}

	memcpy(ptr, this->imageHeader, SIZE_GIF_IMAGE_HEADER);
	ptr+=SIZE_GIF_IMAGE_HEADER;

	//cout << "IMGHEADER" << endl;
	if(LCTSize!=0)
	{
		//cout << "LCT : " << image->getLCTSize() << endl;
		memcpy(ptr, this->LCT, this->LCTSize);
		ptr+=this->LCTSize;
	}
	*ptr=this->LZW;
	ptr++;
	memcpy(ptr, this->dataBlock, this->dataBlockSize);
	ptr+=this->dataBlockSize;
	//cout << "block : " << image->getDataBlockSize();
	return ptr;
}

void GIFLocalImage::writeImageFile(ofstream & output)
{
	if(this->GCE!='\0')
	{
		//cout << "write GCE" << endl;
		output.write((char*)this->GCE, SIZE_GCE);
	}
	output.write((char*)this->imageHeader, SIZE_GIF_IMAGE_HEADER);
	//cout << "write IMGHeader" << endl;

	if(this->LCTSize!=0)
	{
		//cout << "write LCT" << endl;
		output.write((char*)this->LCT, this->LCTSize);
	}
	output.put((char)this->LZW);
	//cout << "write LZW" << endl;
	output.write((char*)this->dataBlock, this->dataBlockSize);
	//cout << "write block" << endl;
}



GIFLocalImage::GIFLocalImage(GIFData* GCT, int GCTSize, bool sorted, GIFData** ptr, GIFData* endp):LCT(NULL), LCTSize(0), dataBlock(NULL), dataBlockSize(0)
{
	GCE[0]='\0';
	bool exitFlag=false;
	int dataBlockSize=0;
	GIFData* blockStart;
	while(!exitFlag){
		//cout << "GIFLocalImage : " << int(**ptr) << endl;//////////////
		switch(**ptr)
		{
			case EXTENSION:
				//cout << "ext" << endl;
				(*ptr)++;
				switch(**ptr)
				{
					case 0xF9:	//Graphics Control Extension Marker, Why can't work with enum GCE??? -> namespace Err.(GIFLocalImage::GCE)
						//cout << "gce" << endl;
						(*ptr)++;
						if(safe((*ptr)+1, **ptr, endp))
						{
							memcpy(this->GCE, (*ptr-2), SIZE_GCE);
							*ptr+=**ptr+2;
						}
						break;
					default:
						cout << "def" << endl;
						(*ptr)++;
						while(**ptr!=TERM)
						{
							*ptr+=**ptr+1;
						}
						(*ptr)++;
						break;
				}
				break;
			case IMAGE:
				//cout << "image" << endl;
				if(safe((*ptr)+1, SIZE_GIF_IMAGE_HEADER, endp))
				{
					memcpy(this->imageHeader, *ptr, SIZE_GIF_IMAGE_HEADER);
					*ptr+=SIZE_GIF_IMAGE_HEADER;
				}
				else{
					cout << "safe fail : imageHeader" << endl;
					exit(EXIT_FAILURE);
				}
				//cout << "imageHeader " << int(**ptr) << endl;///////////////
				if((this->imageHeader[9]&0x80)==0x80)
				{
					//cout << "LCT" << endl;
					this->LCTSize=(3*(2<<(imageHeader[9]&0x07)));
					if(safe(*ptr, this->LCTSize, endp))
					{
						this->LCT=new GIFData[this->LCTSize];
						memcpy(this->LCT, *ptr, this->LCTSize);
						*ptr+=this->LCTSize;
					}
					else{
						cout << "safe fail : LCT" << endl;
						exit(EXIT_FAILURE);
					}
				}
				else{
					imageHeader[9]|=0x80;
					if(sorted)
						imageHeader[9]|=0x20;
					this->LCTSize=GCTSize;
					GCTSize/=3;
					int cnt=0;
					while(GCTSize>2)
					{
						cnt++;
						GCTSize/=2;
					}
					imageHeader[9]|=cnt;
					this->LCT=new GIFData[this->LCTSize];
					memcpy(this->LCT, GCT, this->LCTSize);	
				}
				this->LZW=**ptr;
				(*ptr)++;
				blockStart=*ptr;
				while(**ptr!=TERM)
				{
					if(safe((*ptr)+1, **ptr, endp)){
						dataBlockSize+=**ptr+1;
						*ptr+=**ptr+1;
					}
					else{
						cout << "safe fail : dataBlock" << endl;
						exit(EXIT_FAILURE);
					}
				}
				(*ptr)++;
				dataBlockSize++;
				this->dataBlockSize=dataBlockSize;
				this->dataBlock=new GIFData[this->dataBlockSize];
				memcpy(this->dataBlock, blockStart, this->dataBlockSize);
				this->imageSize=(this->GCE[0]=='\0'?0:SIZE_GCE)+SIZE_GIF_IMAGE_HEADER+this->LCTSize+1+this->dataBlockSize;
				//cout << this->LCTSize << " " << this->dataBlockSize << endl; 
				exitFlag=true;
				break;
			default:
				cout << "default : " << **ptr << endl;
				exitFlag=true;
				break;
		}
	}
}
/*
GIFLocalImage::GIFLocalImage(GIFLocalImage & image):LCT(NULL), LCTSize(0), dataBlock(NULL), dataBlockSize(0)
{
	GCE[0]='\0';
	memcpy(this->GCE, image.GCE, SIZE_GCE);
	memcpy(this->imageHeader, image.imageHeader, SIZE_GIF_IMAGE_HEADER);
	this->LCTSize=image.LCTSize;
	this->dataBlockSize=image.dataBlockSize;
	this->LZW=image.LZW;
	this->LCT=new GIFData[this->LCTSize];
	memcpy(this->LCT, image.LCT, this->LCTSize);

	this->dataBlock=new GIFData[this->dataBlockSize];
	memcpy(this->dataBlock, image.dataBlock, this->dataBlockSize);
	this->imageSize=image.imageSize;
}
*/
GIFLocalImage::~GIFLocalImage()
{
	if(LCT!=NULL)
	{
		delete[] LCT;
		LCT=NULL;
	}

	if(dataBlock!=NULL)
	{
		delete[] dataBlock;
		dataBlock=NULL;
	}
}
