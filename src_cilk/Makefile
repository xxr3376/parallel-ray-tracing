#makefile

CC	=	cilk++
FLAGS	= -O2
# CV_FLAGS = `pkg-config --libs opencv --cflags opencv`
CV_FLAGS = -I/usr/include/opencv -lopencv_core -lopencv_imgproc -lopencv_highgui
SOURCES	=	$(filter-out main.cpp, $(wildcard *.cpp))
OBJS	=	$(patsubst %.cpp, ../bin/%.o, $(SOURCES))
TARGET	=	../bin/prt

all	:	$(OBJS) $(TARGET)

$(TARGET)	:	main.cpp $(OBJS)
	$(CC) $(FLAGS) -o $(TARGET) $< $(OBJS) $(CV_FLAGS)

../bin/%.o	:	%.cpp %.h
	$(CC) -c $(FLAGS) -o $@ $<

show	:
	echo $(OBJS)

.PHONY	:	clean run benchmark

clean	:
	rm -f ../bin/*.o
	rm -f $(TARGET)
	rm -f $(TARGET).exe

run		:
	make all
	$(TARGET)
benchmark :
	gprof $(TARGET) > benchmark
