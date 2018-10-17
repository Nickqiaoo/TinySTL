CC = gcc
CFLAGS = -std=c++11
LDFLAGS = -lgtest -lgtest_main -lpthread -lstdc++
SOURCE = $(wildcard *.cc)
OBJS = $(patsubst %.cc,%,$(SOURCE))
#OBJS = ListTest VectorTest

%.o : %.cc
	$(CC) $(CFLAGS) -c $^ -o $@
$(OBJS) : % : %.o ../Alloc.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
all : $(OBJS)  
clean :
	rm ../Alloc.o *.o $(OBJS)