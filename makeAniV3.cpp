#include <iostream>

#include <Magick++.h>

//#include "makeAniV3.h"
#include "AniGIFMaker.h"

using std::cout;
using std::cin;
using std::endl;

using namespace Magick;

void parseArgs(double &tolerance, bool &cOption, char &mode, char **inputSize, char **outfilename, int *&files, const int argc, char **argv);//
bool isCompatible(const char *input);

int main(int argc, char** argv)
{
	double tolerance=3;
	bool cOption=false;
	char mode='m';
	char *outfilename=NULL;
	char *inputSize=NULL;
	int *files=new int[argc];
	for(int i=0; i<argc; i++) {
		files[i]=0;
	}

	Geometry imgGeo(0, 0);
	Geometry pageGeo(0, 0);

	parseArgs(tolerance, cOption, mode, &inputSize, &outfilename, files, argc, argv);

	AniGIFMaker maker(tolerance, cOption, mode, inputSize);
	
	int cnt=0;
	while(files[cnt]!=0) {
		cout << "argv : " << argv[files[cnt]] << endl;
		maker.addToAniGIF(argv[files[cnt]]);
		cnt++;
	}
	cout << "list : " << maker.getImgList() << endl;
	maker.makeAniGIF(outfilename);

	//DEBUG
	/*
	cout << tolerance << " " << cOption << " " << mode << " ";
	
	if(inputSize!=NULL) {
		cout << *inputSize << " ";
	} else {
		cout << "NULL ";
	}

	if(outfilename!=NULL) {
		cout << *outfilename << " ";
	} else {
		cout << "NONAME ";
	}

	cout << endl;
	*/

	/*
	for(int i=0; i<argc; i++)
	{
		cout << files[i] << " ";
	}	
	cout << endl;
	*/

	delete[] files;
	return 0;
}

void parseArgs(double &tolerance, bool &cOption, char &mode, char **inputSize, char **outfilename, int *&files, const int argc, char **argv)
{
	int fileCnt=0;

	for(int i=1; i<argc; i++) {
		if(argv[i][0]=='-') {
			switch(argv[i][1]) {
				case 'c':
					cOption=true;
					break;
				case 'w':
				case 'h':
				case 'm':
				case 'f':
				case 'a':
					mode=argv[i][1];
					*inputSize=argv[i+1];
					if(!isCompatible(*inputSize)) {
						cout << "Incompatible Size Error. Exiting..." << endl;
						exit(EXIT_FAILURE);
					}
					i++;
					break;

				case 't':
					if(!isCompatible(argv[i+1])) {
						cout << "Incompatible Tolerance Error. Exiting..." << endl;
						exit(EXIT_FAILURE);
					}
					tolerance=atof(argv[i+1]);
					if(tolerance<1) {
						tolerance = 1/tolerance;
					}
					i++;
					break;
				case 'o': 
					*outfilename=argv[i+1];
					i++;
					break;	
				default:
					cout << "Not In Option! Ignoring..." << endl;
					break;
			}
		} else {
			cout << "Default Argv" << endl;
			files[fileCnt++]=i;
		}
	}
}

bool isCompatible(const char *input)
{
	char copy[21];
	strncpy(copy, input, 20);
	char *token=strtok(copy, "x");
	double number=atof(token);
	if(number<=0) {
		return false;
	}
	return true;
}
