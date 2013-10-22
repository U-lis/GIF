ALL = GIFClass.cpp GeoCalculator.cpp AniGIFMaker.cpp makeAniV3.cpp 

new : GIFLocal.cpp GIF.cpp
	g++ -c GIFLocal.cpp GIF.cpp

once : $(ALL)
	c++ `Magick++-config --cxxflags --cppflags` -g -o makeAniV3 $(ALL) `Magick++-config --ldflags --libs`
	cp makeAniV3 TEST

clean:
	rm -rf *.o *.gch makeAniV3

name:
	./makeAniV3 -w 400 -o test.gif small.jpg

noname:
	./makeAniV3 -w 400 small.jpg
