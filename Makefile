CC=g++
FLAGS = -std=c++14 -g -O3

files = percolation.o EasyBMP.o

all: driver sweep

# Executables
driver: driver.o $(files)
	$(CC) $(OPT) $^ -o $@

sweep: sweep.o $(files)
	$(CC) $(OPT) $^ -o $@

# Object files
EasyBMP.o : EasyBMP/EasyBMP.cpp
	$(CC) -c $(CFLAGS) $<

%.o : %.cpp
	$(CC) -c $(FLAGS) $^

.PHONY: clean
clean:
	rm *.o $(targets)