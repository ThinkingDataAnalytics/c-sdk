CC=gcc
CFLAGS=-Wall -W -Iinclude -DUSE_POSIX -fPIC

all: libthinkingdata
	mkdir -p ./output/include ./output/lib
	cp -r include ./output
	cp libthinkingdata.a ./output/lib/.

libthinkingdata: json.o util.o list.o thinkingdata.o http_client.o
	ar rcs libthinkingdata.a json.o util.o list.o thinkingdata.o http_client.o
	mkdir -p ./output/include ./output/lib
	cp include/thinkingdata.h ./output/include/.
	cp libthinkingdata.a ./output/lib/.

sdk-demo: thinkingdata.o json.o util.o list.o http_client.o
	$(CC) -o $@ demo/demo.c $^ $(CFLAGS)

thinkingdata.o: src/thinkingdata.c
	$(CC) -c src/thinkingdata.c $(CFLAGS)

list.o: src/list.c
	$(CC) -c src/list.c $(CFLAGS)

util.o: src/util.c
	$(CC) -c src/util.c $(CFLAGS)

json.o: src/json.c
	$(CC) -c src/json.c $(CFLAGS)

http_client.o: src/http_client.c src/http_client.c
	$(CC) -c src/http_client.c $(CFLAGS)

.PHONY: clean
	
clean:
	rm -rf *.o *.a
	rm -rf sdk-demo
	rm -rf output