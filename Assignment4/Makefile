current_dir = $(shell pwd)
CFLAGS = -I$(current_dir)

CC = g++

% : %.cpp
	$(CC) -std=c++11 $(CFLAGS) -o $@ $<  $(LIBS)

clean:
	rm -f Output
	rm -f Simulator
	rm -f test
