SRCDIR = .
BINDIR = 
PRODUCT_FLAGS =

SOURCES = \
	entity.cpp\
	window.cpp\
	test.cpp

OBJECTS = $(SOURCES:%.cpp=%.o)

CXX = g++

INCLUDES = -I $(SRCDIR)/include

CXXFLAGS = -Wall -Wextra ${INCLUDES} `pkg-config gtk+-2.0 --cflags`

LDLIBS = `pkg-config gtk+-2.0 --libs`

LDFLAGS = -lm -lrt -lpthread 

%.o: %.cpp
	$(CXX)  $(PRODUCT_FLAGS) $(CXXFLAGS)  -c -o $@ $<

all: cellutron

cellutron: $(OBJECTS)
	$(CXX) -o $(BINDIR)$@ $(OBJECTS) $(LDLIBS) $(LDFLAGS)

all:
	@echo "============================================================================================="; \
	echo "cellutron successfully built"

clean:
	/bin/rm -f $(OBJECTS) $(BINDIR)cellutron  *~

