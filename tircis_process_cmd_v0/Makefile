#CC = /usr/bin/arm-linux-gnueabihf-g++ -g  --static
#cc = /usr/bin/arm-linux-gnueabihf-gcc -g --static  
#LD = /usr/bin/arm-linux-gnueabihf-ld
CC = g++  
cc = gcc  
LD = ld

# Set to compiler flags
FLAGS = -O0
CFLAGS = ${FLAGS}
CCFLAGS = ${FLAGS}  -std=c++11


# List of object files to generate
OBJS = mainprocess.o \
       geneprocess.o \
       main.o \
       suchi_offsets.o \
       wavecal.o \
       suchifile.o \
       suchi_utils.o \
       poly.o \
       svbksb_d.o \
       svdcmp_d.o \
       svdfit_d.o \
       nrutil.o \
       png.o \
       cvprocess.o

# Executable file to generate
EXECUTABLE = tircis_process_cmd

#INCLUDES =  -I/home/hg/gumstix/OpenCV-2.3.1/build/install/include -I/home/hg/gumstix/fftw-3.3.4/build/include 
# temporarily removed -lzlib from list for centos build

# Required libraries
LIBS =  -lm  -lfftw3 -lpthread -lpng -lopencv_legacy -lopencv_ml -lopencv_features2d -lopencv_highgui -lopencv_imgproc -lopencv_core -lpthread 


# Compilation rules
.cpp.o  :
	${CC} ${CCFLAGS} -c ${INCLUDES}  $*.cpp
.c.o  :
	${cc} ${CFLAGS}  -c ${INCLUDES}  $*.c


# Linking
${EXECUTABLE} : ${OBJS} ${OBJS1} 
	${CC} ${CCFLAGS} ${OBJS} -o ${EXECUTABLE} ${LIBS}
	/bin/cp ${EXECUTABLE} ../testdata/

default: ${EXECUTABLE}

clean:
	/bin/rm -f ${OBJS} ${EXECUTABLE}
	/bin/rm -f ../testdata/${EXECUTABLE}
