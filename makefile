CPP = "D:\mingw64\bin\g++.exe"
CXXFLAGS = \
	-Wall \
	-Werror \
	-Wpedantic \
	-O2 \
	-std=c++14 \
	-ID:/boost_1_82_0/include/boost-1_82 \
	-ID:/boost_1_82_0/lib

OBJECTS = \
	obj/DatPreafix.o

LIB = \
	-LD:/boost_1_82_0/lib

BOOSTLIB = \
	-lboost_filesystem-mgw13-mt-d-x64-1_82 \
	-lboost_system-mgw13-mt-d-x64-1_82

.PHONEY: all clean

all: obj/DatPraefix.o
	$(CPP) $(CXXFLAGS) $^ -o DatPraefix.exe -s $(LIB) $(BOOSTLIB)

obj/DatPraefix.o: DatPraefix.cpp
	-if not exist obj mkdir obj
	$(CPP) $(CXXFLAGS) -c $^ -o $@


clean:
	-if exist DatPraefix.exe del DatPraefix.exe
	-if exist obj\DatPraefix.o del obj\DatPraefix.o
