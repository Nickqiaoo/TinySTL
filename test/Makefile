CC = gcc
CFLAGS = -std=c++11
LDFLAGS = -lgtest -lgtest_main -lpthread -lstdc++
objs = ListTest VectorTest

%.o : %.cc
	$(CC) $(CFLAGS) -c $^ -o $@
$(objs) : % : %.o ../Alloc.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
all : $(objs)  
clean :
	rm ../Alloc.o *.o *Test
	