GEO_CALCULATOR = GeoCalculator.h GeoCalculator.cpp
ANI_GIF_MAKER = AniGIFMaker.h AniGIFMaker.cpp GeoCalculator.o
GIF_CLASS = GIFClass.h GIFLocalImage.h GIFClass.cpp
FILES = makeAniV3.cpp GIFClass.o AniGIFMaker.o GeoCalculator.o

makeAniV3 : $(FILES)
	c++ `Magick++-config --cxxflags --cppflags` -g -o $@ $(FILES) `Magick++-config --ldflags --libs`
	cp makeAniV3 test

AniGIFMaker.o : $(ANI_GIF_MAKER)
	c++ `Magick++-config --cxxflags --cppflags` -c $(ANI_GIF_MAKER) `Magick++-config --ldflags --libs`

GeoCalculator.o : $(GEO_CALCULATOR)
	c++ `Magick++-config --cxxflags --cppflags` -c $(GEO_CALCULATOR) `Magick++-config --ldflags --libs`

GIFClass.o : $(GIF_CLASS)
	g++ -c $(GIF_CLASS)
