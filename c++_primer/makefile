CC=g++
FlagsOne=-Wall -O0 -std=c++11 -ggdb
FlagsTwo=-Wl,--no-as-needed -lpthread

CPPFLAGS=${FlagsOne} ${FlagsTwo}
all: commands, agent

%.o: %.cc
	$(CC) $(CCFLAGS) $(LOCFLAGS) -c $< -o $@
%: %.o
	$(CC) $(CCFLAGS) $(LOCFLAGS) $< -o $@


commands: commands.o daemon.o
	g++ commands.o daemon.o -o commands
agent: agent.o daemon.o
	g++  agent.o daemon.o -o agent

udpcli: udpcli.o daemon.o
	g++ udpcli.o daemon.o -o udpcli

udpcli.o: udpcli.cc
	${CC} -c $< ${FlagsOne} -o $@

agent.o: agent.cc
	${CC} -c $< ${FlagsOne} -o $@

daemon.o: daemon.cc
	${CC} -c $< ${FlagsOne} -o $@

commands.o: commands.cc
	${CC} -c $< ${FlagsOne} -o $@

clean:
	rm -rf *.o
	rm -rf commands agent

