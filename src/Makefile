$(CC) = nvcc

COPTS = -g -std=c++11 -Wall -Wno-deprecated-declarations -Wextra  

INC = -I./libs -I/opt/cuda/include -I/usr/lib/glut-3.7/include -I../common/inc/ \
 -I../common/inc/GL

LIBS = -lstdc++ -lglut -lGLU -lGL -lGLEW -lm

OBJS = \
body.o

.cpp.o:
	 $(CC) -c $(INC) $(COPTS) -o $@ $<

all: autorun

autorun: $(OBJS) autorun.o
	$(CC) -o $@ $(INC) $(COPTS) $(OBJS) autorun.o $(LIBDIR) $(LIBS)

clean:
	-rm -f *.o
	-rm -f autorun
