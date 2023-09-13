CXX = g++

EXE = bb_save
MAIN = main.cpp
CXXFLAGS = -std=c++14 -Wall -Wformat

$(EXE): $(MAIN)
	$(CXX) -o $@ $(MAIN) $(CXXFLAGS)

clean:
	del $(MAIN)