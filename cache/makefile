CC=g++
CPPFLAGS=-O0 -std=c++11 -ggdb
#CPPFLAGS=-Wall -O0 -std=c++11 -ggdb
#CPPFLAGS=-O0 -std=c++11 -ggdb
OBJ=daemon slab item

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
item:item.cpp
	${CC} ${CPPFLAGS} item.cpp -lpthread  -o item

slab: conf.o slab.o
	${CC} ${CPPFLAGS} conf.o slab.o -o slab

conf.o:conf.cpp
	${CC} ${CPPFLAGS} -c conf.cpp -o conf.o
slab.o:slab.cpp
	${CC} ${CPPFLAGS} -c slab.cpp -o slab.o



clean:
	rm -rf *.o
	rm -rf ${OBJ}
