#ifndef GEO_CALCULATOR_H
#define GEO_CALCULATOR_H

#include <iostream>
#include <Magick++.h>
#include <cstdio>

using namespace Magick;

class GeoCalculator {
	private:
		char inputSize[21];
		double tolerance;
		int geoWidth;
		int geoHeight;
		double maxRatio;
		double minRatio;

		void parseInputSize(const char mode, const char *inputSize);
		//int calcSize(int size, double ratio);

		void setWidthOption(Image *&image, Geometry &imgGeo, Geometry &pageGeo);
		void setHeightOption(Image *&image, Geometry &imgGeo, Geometry &pageGeo);
		void setFixedOption(Image *&image, Geometry &imgGeo, Geometry &pageGeo);
		void setMobileOption(Image *&image, Geometry &imgGeo, Geometry &pageGeo);
		void setAutoOption(Image *&image, Geometry &imgGeo, Geometry &pageGeo);

		void setNoresize(Image *&image, Geometry &imgGeo, Geometry &pageGeo);

	public:
		GeoCalculator(double tolerance);
		~GeoCalculator();
		
		void calcGeo(Geometry &imgGeo, Geometry &pageGeo, Image *&image, char &mode, char *&inputSize);
};

#endif	// GEO_CALCULATOR_H
