CC=g++
CPPFLAGS=-Wall -O0 -std=c++11 -ggdb

%.o: %.cc
	$(CC) $(CCFLAGS) $(LOCFLAGS) -c $< -o $@
%: %.o
	$(CC) $(CCFLAGS) $(LOCFLAGS) $< -o $@


