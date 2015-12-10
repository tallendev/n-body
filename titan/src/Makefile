CC = nvcc

.SUFFIXES: .cpp .c .h .o .cu

COPTS = -g -std=c++11
#-lprofiler #-O2 #-ftz=true

ifdef DEBUG
DBG = -Dcudaerr
endif

INC = -I/home/tnallen/lib -I./lib -I/opt/cuda/include  -I../common/inc/ -I../common/inc/GL -I/home/tnallen/include

LIBS = -lstdc++ -lglfw -lGLU -lGL -lGLEW -lm

OBJS = \
body.o \
motion.o \
utils.o

.cpp.o:
	 $(CC) -c $(LIBS) $(INC) $(DBG) $(COPTS) -o $@ $<

.cu.o:
	 $(CC) -c $(LIBS) $(INC) $(DBG) $(COPTS) -o $@ $< -dc

all: autorun

debug: clean
	$(MAKE) DEBUG=1

autorun: $(OBJS) autorun.o
	$(CC) -o $@ $(INC) $(DBG) $(COPTS) $(OBJS) autorun.o $(LIBS)

clean:
	-rm -f *.o
	-rm -f autorun
