CC=g++
FlagsOne=-Wall -O0 -std=c++11 -ggdb
FlagsTwo=-Wl,--no-as-needed -lpthread

CPPFLAGS=${FlagsOne} ${FlagsTwo}
%.o: %.cc
	$(CC) $(CCFLAGS) $(LOCFLAGS) -c $< -o $@
%: %.o
	$(CC) $(CCFLAGS) $(LOCFLAGS) $< -o $@


