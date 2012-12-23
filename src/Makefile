#makefile

CC	=	g++
FLAGS	=	-O1 -c
SOURCES	=	$(wildcard *.cpp)
OBJS	=	$(patsubst %.cpp, %.o, $(SOURCES))

all	:	$(OBJS)

%.o	:	%.cpp %.h
	$(CC) $(FLAGS) -o $@ $<

.PHONY	:	clean

clean	:
	rm -f *.o
