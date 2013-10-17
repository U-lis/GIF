#include <Magick++.h>
#include "gifV2.h"
#include <cmath>

using namespace Magick;
using std::cout;
using std::endl;
using std::list;
using std::string;
using std::exception;

void calcResizeGeo(Geometry & imgGeo, Geometry & pageGeo,  Image* img, char mode, const char* param);
void resize(Image* image, Geometry & imgResizeGeo, Geometry & pageResizeGeo, const char mode, const char* param, Geometry & maxGeo, list<GIF*> & endList);
int inline calcSize(int i1, double d2){return int(floor(i1*d2+0.5));}

int main(int argc, char** argv)
{
	ofstream log("makeAniV2.log");
	if(!log.good())
	{
		cout << "log File Open Failed" << endl;
		exit(EXIT_FAILURE);
	}
	log << "log Start" << endl;
	char str[30];
	sprintf(str, "/proc/%d/status", getpid());
	ifstream in(str);
	if(!in)
	{
		cout << "Process Status File Open Failed" << endl;
		exit(EXIT_FAILURE);
	}

	double startTime=clock();

	cout.setf(ios::hex, ios::basefield);
	cout << "argc : " << argc << endl;

	char mode=argv[1][0];
	char* param=&argv[1][1];
	bool cut=false;
	if(mode=='c')
	{
		cut=true;
		mode=argv[1][1];
		param=&argv[1][2];
	}
	Image* image=NULL;
	int resizeCnt=0;
	Geometry maxGeo(0, 0);
	Geometry imgResizeGeo;
	Geometry pageResizeGeo;

	list<GIF*> endList;


	for(int i=2; i<argc; i++)
	{
		cout << argv[i] << endl;
		string arg=argv[i];
		string suffix=arg.substr(arg.rfind(".")+1);

		if(suffix.compare("gif")==0 || suffix.compare("GIF")==0)
		{
			cout << "GIF comes!" << endl;
			GIF GIFImg(argv[i]);
			list<GIFImage*> & GIFImgList=GIFImg.getImageList();
			list<GIFImage*>::iterator itr=GIFImgList.begin();
			cout << "GIF Read Done" << endl;

			for(int i=0; itr!=GIFImgList.end(); i++, itr++)
			{
				int fileSize=GIFImg.getGIFSize()+(*itr)->getImageSize();
				//cout << "fileSize : " << fileSize << endl;
				GIFData* tmpStream=new GIFData[fileSize];
				GIFImg.writeBlob(tmpStream, i);
				Blob* blob=new Blob(tmpStream, fileSize);
				/*for(int t=0; t<fileSize+1; t++)
				{
					cout << int(tmpStream[t]) << " ";
					if(t%26==0 && t>1)
						cout << endl;
				}
				cin.get();
*/
				image=new Image(*blob);
				delete blob;
				delete[] tmpStream;

				if(i==GIFImg.getImageCnt()-1)
					image->gifDisposeMethod(2);
				if(GIFImg.getImageList().size()==1 || cut)
					image->animationDelay(100);
				resize(image, imgResizeGeo, pageResizeGeo, mode, param, maxGeo, endList);

				in.seekg(ios::beg);
				for(int t=0; t<16; t++)
					in.getline(str, 100);
				log << "resized : " << str << endl;
				if(image!=NULL)
				{
					delete image;
					image=NULL;
				}
				if(cut)
					break;
			}
		}
		else
		{
			//cout << "else" << endl;
			image=new Image(argv[i]);
			resize(image, imgResizeGeo, pageResizeGeo, mode, param, maxGeo, endList);

			in.seekg(ios::beg);
			for(int t=0; t<16; t++)
				in.getline(str, 100);
				log << "resized : " << str << endl;

				if(image!=NULL)
				{
					delete image;
					image=NULL;
				}
		}
		log << argv[i] << " Resize Done" << endl;
	}	// end argc : reading input file
	cout << "resize done" << endl;
	list<GIF*>::iterator enditr=endList.begin();
	char name[30];
	/*for(int i=0; enditr!=endList.end(); i++, enditr++)
	{
		sprintf(name, "asdf_%03d.gif", i);
		(*enditr)->writeFile(name, ALLIMGS);
	}*/
	GIF merged(endList);
	merged.writeFile("animated_maker.gif", ALLIMGS);
	in.seekg(ios::beg);
	for(int t=0; t<16; t++)
		in.getline(str, 100);
	log << endl << "Process Done : " << str << endl;

	for(list<GIF*>::iterator endItr=endList.begin(); endItr!=endList.end(); endItr++)
		delete *endItr;

	log << "Process Time : " << ((clock()-startTime)/CLOCKS_PER_SEC) << endl;
	in.close();
	log.close();
	cout << "Process Done" << endl;
	return 0;
}

void calcResizeGeo(Geometry & imgGeo, Geometry & pageGeo, Image* img, char mode, const char* param)
{
	char par[12];
	strncpy(&par[0], param, 11);
	char* pparam=strtok(par, "x");
	double param1=atof(pparam);
	double param2=-1;
	pparam=strtok(NULL, "x");
	if(pparam!=NULL)
		param2=atof(pparam);

	Geometry* geo=NULL;
	Geometry aGeo;
	double ratio=0;
	double minRatio=param1/param2/3, maxRatio=param1/param2*3;

	if(param1!=0 && param2!=0)
	{
		switch(mode)
		{
			case 'w':
				ratio=param1/img->page().width();
				imgGeo=Geometry(calcSize(img->columns(), ratio), 0);
				pageGeo=Geometry(calcSize(img->page().width(), ratio), calcSize(img->page().height(), ratio), calcSize(img->page().xOff(), ratio), calcSize(img->page().yOff(), ratio));
				break;

			case 'h':
				ratio=param1/img->page().height();
				imgGeo=Geometry(0, calcSize(img->rows(), ratio));
				pageGeo=Geometry(calcSize(img->page().width(), ratio), calcSize(img->page().height(), ratio), calcSize(img->page().xOff(), ratio), calcSize(img->page().yOff(), ratio));
				break;

			case 'a':
				ratio=double(img->page().width())/double(img->page().height());
				cout << "a option : " << minRatio << " " << maxRatio << " " << ratio << endl;
				if(ratio<minRatio && img->page().height()>param2)	// Image is too long
				{
					cout << "long!" << endl;
					aGeo=Geometry(int(img->page().width()), int((img->page().width()*3)<param2?param2:(img->page().width()*3)));
					aGeo.xOff(int((img->page().width()-aGeo.width())/2));
					aGeo.yOff(int((img->page().height()-aGeo.height())/2));
				}
				else if(ratio>maxRatio && img->page().width()>param1)	// Image is too wide
				{
					cout << "wide!" << endl;
					aGeo=Geometry(int((img->page().height()*3)<param1?param1:(img->page().height()*3)), int(img->page().height()));
					aGeo.xOff(int((img->page().width()-aGeo.width())/2));
					aGeo.yOff(int((img->page().height()-aGeo.height())/2));
				}
				if(!(ratio<maxRatio && ratio>minRatio))
				{
					cout << "aGeo : " << aGeo.width() << " " << aGeo.height() << " " << aGeo.xOff() << " " << aGeo.yOff() << endl;
					img->crop(aGeo);
					aGeo.xOff(img->page().xOff()-aGeo.xOff());
					aGeo.yOff(img->page().yOff()-aGeo.yOff());
					img->page(aGeo);
				}
				cout << "pre-crop : " << img->page().width() << " " << img->page().height() << endl;
			default:
				if(mode!='a')
					cout << "Wrong Resize Mode, Set to Default mode m" << endl;
			case 'm':
				ratio=(img->page().width()/param1>img->page().height()/param2 ? param1/img->page().width() : param2/img->page().height());
				imgGeo=img->page().width()/param1>img->page().height()/param2 ? Geometry(calcSize(img->columns(), ratio), 0) : Geometry(0, calcSize(img->rows(), ratio));
				pageGeo=Geometry(calcSize(img->page().width(), ratio), calcSize(img->page().height(), ratio), calcSize(img->page().xOff(), ratio), calcSize(img->page().yOff(), ratio));
				break;
			case 'f':
				ratio=(param1/img->page().width()>param2/img->page().height() ? param1/img->page().width() : param2/img->page().height());
				imgGeo=param1/img->page().width()>param2/img->page().height() ? Geometry(calcSize(img->columns(), ratio), 0) : Geometry(0, calcSize(img->rows(), ratio));
				pageGeo=Geometry(calcSize(img->page().width(), ratio), calcSize(img->page().height(), ratio), calcSize(img->page().xOff(), ratio), calcSize(img->page().yOff(), ratio));
				cout << "imgGeo : " << imgGeo.width() << " " << imgGeo.height() << endl;
				break;
		}
		if(mode!='f' && (imgGeo.width()>img->page().width() || imgGeo.height()>img->page().height()))
		{
			imgGeo=Geometry(0, 0, 0, 0);
			pageGeo=Geometry(0, 0, 0, 0);
			imgGeo.greater(MagickTrue);
			imgGeo.less(MagickFalse);
			pageGeo.greater(MagickTrue);
			pageGeo.less(MagickFalse);
		}
	}
}

void resize(Image* image, Geometry & imgResizeGeo, Geometry & pageResizeGeo, const char mode, const char* param, Geometry & maxGeo, list<GIF*> & endList)
{
	calcResizeGeo(imgResizeGeo, pageResizeGeo, image, mode, param);

	if(image->magick()!="GIF")
	{
		//cout << "setting" << endl;
		image->gifDisposeMethod(2);
		image->animationDelay(100);
		//image->matte(MagickFalse);
		image->magick("GIF");
	}
	image->animationIterations(0);

	if((imgResizeGeo.width()==0 && imgResizeGeo.height()==0) || (pageResizeGeo.width()==0 && pageResizeGeo.height()==0))
	{
		cout << "noresize" << endl;
	}
	else
	{
		image->resize(imgResizeGeo);
		image->page(pageResizeGeo);
	}
		try
		{
			if(mode=='f')
			{
				Geometry cropGeo(param);
				cropGeo.xOff((image->page().width()-cropGeo.width())/2);
				cropGeo.yOff((image->page().height()-cropGeo.height())/2);
				image->crop(cropGeo);
				cropGeo.xOff(image->page().xOff()-cropGeo.xOff());
				cropGeo.yOff(image->page().yOff()-cropGeo.yOff());
				image->page(cropGeo);
			}
		}
		catch(Exception e)
		{
			cout <<	"ERR!!!  " <<e.what() << endl;
			return;
		}
	//cout << "resize" << endl;

	if(maxGeo.width()<image->page().width())
		maxGeo.width(image->page().width());
	if(maxGeo.height()<image->page().height())
		maxGeo.height(image->page().height());
	//cin.get();
	Blob* endBlob=new Blob();
	image->write(endBlob);
	//cout << "writeBlob" << endl;
	//image->write("asdf.gif");
	endList.push_back(new GIF((GIFData*)endBlob->data(), endBlob->length()));
	//cout << "endList : " << endList.size() << endl;
	delete endBlob;
}
