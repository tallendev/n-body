CC = g++

.SUFFIXES: .cpp .c .h .o .cu

COPTS = -g -std=c++11 

INC = -I./libs -I/opt/cuda/include  -I../common/inc/ -I../common/inc/GL

LIBS = -lstdc++ -lglfw -lGLU -lGL -lGLEW -lm

OBJS = \
body.o \
motion.o

.cpp.o:
	 $(CC) -c $(INC) $(COPTS) -o $@ $<

.cu.o:
	 nvcc -c $(INC) $(COPTS) -o $@ $<

all: autorun

autorun: $(OBJS) autorun.o
	nvcc -o $@ $(INC) $(COPTS) $(OBJS) autorun.o $(LIBDIR) $(LIBS)

clean:
	-rm -f *.o
	-rm -f autorun
