SIM_OBJ_FILES_DIR=/data/gent/421/vsc42150/NU-GPU/GPGPU-sim_HBM_LLC_CTA_First_Touch_Even_Combined_RR/build/src/
OUTPUT_DIR=$(SIM_OBJ_FILES_DIR)/gpuwattch
TARGET = mcpat
SHELL = /bin/sh
.PHONY: all depend clean
.SUFFIXES: .cc .o

ifndef NTHREADS
  NTHREADS = 4
endif


LIBS = -I/usr/lib/ -I/usr/lib64/
INCS = -lm

CC=
CXX=

ifeq ($(shell getconf LONG_BIT),64) 
	CXX = g++ -m64
	CC  = gcc -m64
else 
	CXX = g++ -m32
	CC  = gcc -m32
endif 

ifeq ($(TAG),dbg)
  DBG = -Wall 
  OPT = -ggdb -fPIC -g -O0 -DNTHREADS=1 -Icacti -lz
else
  DBG = 
  OPT = -O3 -fPIC -msse2 -mfpmath=sse -DNTHREADS=$(NTHREADS) -Icacti -lz
  #OPT = -O0 -DNTHREADS=$(NTHREADS)
endif

#CXXFLAGS = -Wall -Wno-unknown-pragmas -Winline $(DBG) $(OPT) 
CXXFLAGS = -Wno-unknown-pragmas $(DBG) $(OPT) 




VPATH = cacti

SRCS  = \
  ./cacti/Ucache.cc \
  XML_Parse.cc \
  ./cacti/arbiter.cc \
  ./cacti/area.cc \
  array.cc \
  ./cacti/bank.cc \
  ./cacti/basic_circuit.cc \
  basic_components.cc \
  ./cacti/cacti_interface.cc \
  ./cacti/component.cc \
   core.cc \
  ./cacti/crossbar.cc \
  ./cacti/decoder.cc \
  ./cacti/htree2.cc \
  interconnect.cc \
  ./cacti/io.cc \
  iocontrollers.cc \
  logic.cc \
  ./cacti/main.cc \
  ./cacti/mat.cc \
  memoryctrl.cc \
  noc.cc \
  nuca.cc \
  parameter.cc \
  processor.cc \
  router.cc \
  sharedcache.cc \
  subarray.cc \
  ./cacti/technology.cc \
  ./cacti/uca.cc \
  ./cacti/wire.cc \
  xmlParser.cc \
  gpgpu_sim_wrapper.cc \



OBJS = $(patsubst %.cc,$(OUTPUT_DIR)/%.o,$(SRCS))

all: $(OUTPUT_DIR)/$(TARGET)

$(OUTPUT_DIR)/$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) -o $@ $(INCS) $(CXXFLAGS) $(LIBS) -pthread

#obj_$(TAG)/%.o : %.cc
#	$(CXX) -c $(CXXFLAGS) $(INCS) -o $@ $<

$(OUTPUT_DIR)/%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUTPUT_DIR)/Makefile.makedepend: depend

depend:
	touch $(OUTPUT_DIR)/Makefile.makedepend
	makedepend -f$(OUTPUT_DIR)/Makefile.makedepend -p$(OUTPUT_DIR)/ $(SRCS) 2> /dev/null
	$(MAKE) -C ./cacti/ depend

clean:
	-rm -f *.o $(TARGET)
	rm -f Makefile.makedepend Makefile.makedepend.bak

include $(OUTPUT_DIR)/Makefile.makedepend
