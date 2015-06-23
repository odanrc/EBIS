# Paths
OPENCV_PATH=/usr/local

# Programs
CC=
CXX=g++

# Flags
ARCH_FLAGS=
CFLAGS=-Wextra -Wall -pedantic-errors $(ARCH_FLAGS) -O3 -Wno-long-long
LDFLAGS=$(ARCH_FLAGS)
DEFINES=
INCLUDES=-I$(OPENCV_PATH)/include -Iinclude/
LIBRARIES=-L$(OPENCV_PATH)/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lSDL2 -lX11

# Files which require compiling
SOURCE_FILES=\
	src/FaceTracker/IO.cc\
	src/FaceTracker/PDM.cc\
	src/FaceTracker/Patch.cc\
	src/FaceTracker/CLM.cc\
	src/FaceTracker/FDet.cc\
	src/FaceTracker/PAW.cc\
	src/FaceTracker/FCheck.cc\
	src/FaceTracker/Tracker.cc\
	src/Pong/Pong.cc\
	src/eyeLike/helpers.cc\
	src/eyeLike/findEyeCenter.cc\
	src/SmoothingFilter.cc\
	src/DisplayControl/DotController.cc\
	src/DisplayControl/ZoomGrid.cc\
	src/DisplayControl/SDLController.cc\
	src/DisplayControl/DisplayController.cc\
	src/CommandHandler.cc\
	src/liblbp.cc\
	src/Flandmark.cc\
	src/EBIS.cc

# Source files which contain a int main(..) function
SOURCE_FILES_WITH_MAIN=src/exe/main.cc

# End Configuration
SOURCE_OBJECTS=$(patsubst %.cc,%.o,$(SOURCE_FILES))

ALL_OBJECTS=\
	$(SOURCE_OBJECTS) \
	$(patsubst %.cc,%.o,$(SOURCE_FILES_WITH_MAIN))

DEPENDENCY_FILES=\
	$(patsubst %.o,%.d,$(ALL_OBJECTS)) 

all: bin/EBIS

%.o: %.cc Makefile
	@# Make dependecy file
	$(CXX) -MM -MT $@ -MF $(patsubst %.cc,%.d,$<) $(CFLAGS) $(DEFINES) $(INCLUDES) $<
	@# Compile
	$(CXX) $(CFLAGS) $(DEFINES) -c -o $@ $< $(INCLUDES)

-include $(DEPENDENCY_FILES)

bin/EBIS: $(ALL_OBJECTS)
	$(CXX) $(LDFLAGS)  -o $@ $(ALL_OBJECTS) $(LIBRARIES)

.PHONY: clean
clean:
	@echo "Cleaning"
	@for pattern in '*~' '*.o' '*.d' ; do \
		find . -name "$$pattern" | xargs rm ; \
	done
