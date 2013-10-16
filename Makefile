FILES = makeAniV3.cpp

makeAniV3 : $(FILES)
	c++ `Magick++-config --cxxflags --cppflags` -g -o $@ $(FILES) `Magick++-config --ldflags --libs`
