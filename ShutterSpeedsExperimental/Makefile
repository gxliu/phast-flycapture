# ShutterSpeedsExperimental makefile
# To compile the debug verison need to overwrite CXXFLAGS variable to include -ggdb

CC = g++ 
CFLAGS=`pkg-config --cflags --libs  glib-2.0`
OUTPUTNAME = ShutterSpeedsExperimental${D}
INCLUDE = -I../../include -I/usr/include/flycapture \
			-I/usr/include/glib-2.0
LIBS = -L../../lib -lflycapture${D} 

OUTDIR = ../../bin

OBJS = ShutterSpeedsExperimental.o FrameRateCounter.o

${OUTPUTNAME}: ${OBJS}
	${CC} -o ${OUTPUTNAME} ${OBJS} ${LIBS} ${COMMON_LIBS} 
	mv ${OUTPUTNAME} ${OUTDIR}

%.o: %.cpp
	${CC} ${CFLAGS} ${INCLUDE} -c $*.cpp
	
clean_obj:
	rm -f ${OBJS}	@echo "all cleaned up!"

clean:
	rm -f ${OUTDIR}/${OUTPUTNAME} ${OBJS}	@echo "all cleaned up!"
