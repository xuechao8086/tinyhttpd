all: httpd client threadexample

httpd: httpd.c
		#gcc -W -Wall -lsocket -lpthread -o httpd httpd.c
		gcc -W -Wall httpd.c -lpthread -o httpd 


client: client.c
		#gcc -W -Wall -lsocket client.c -o client
		gcc -W -Wall client.c -o client
	
threadexample: threadexample.c
		gcc -W -Wall threadexample.c -o threadexample -lpthread

clone: clone.c
		gcc -W -Wall clone.c -o clone

clean:
		rm httpd client threadexample
