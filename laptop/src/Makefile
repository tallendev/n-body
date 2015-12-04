CC = nvcc

.SUFFIXES: .cpp .c .h .o .cu

COPTS = -g -std=c++11 -lprofiler #--maxrregcount=50#-O2 #-ftz=true

ifdef DEBUG
DBG = -Dcudaerr
endif

INC = -I./libs -I/opt/cuda/include  -I../common/inc/ -I../common/inc/GL

LIBS = -lstdc++ -lglfw -lGLU -lGL -lGLEW -lm

OBJS = \
body.o \
motion.o \
utils.o

.cpp.o:
	 $(CC) -c $(INC) $(DBG) $(COPTS) -o $@ $<

.cu.o:
	 $(CC) -c $(INC) $(DBG) $(COPTS) -o $@ $< -dc

all: autorun

debug: 
	$(MAKE) DEBUG=1

autorun: $(OBJS) autorun.o
	$(CC) -o $@ $(INC) $(DBG) $(COPTS) $(OBJS) autorun.o $(LIBDIR) $(LIBS)

clean:
	-rm -f *.o
	-rm -f autorun
