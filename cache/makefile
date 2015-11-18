CC=g++
CPPFLAGS=-Wall -O0 -std=c++11 -ggdb
OBJ=daemon

daemon: daemon.cpp
	${CC} ${CPPFLAGS} daemon.cpp -o daemon
cache: cache.cpp
	${CC} ${CPPFLAGS} -lpthread cache.cpp -o cache
jenkins_hash: jenkins_hash.cpp
	${CC} ${CPPFLAGS} jenkins_hash.cpp -o jenkins_hash
murmur3_hash: murmur3_hash.cpp
	${CC} ${CPPFLAGS} murmur3_hash.cpp -o murmur3_hash
util:util.cpp
	${CC} ${CPPFLAGS} util.cpp -o util 
clean:
	rm -rf *.o
	rm -rf ${OBJ}
