CC=gcc
CFLAGS=-Wall -W -I. -DUSE_POSIX -fPIC

all: sdk-demo
	ar rcs libthinkingdata.a json.o util.o list.o thinkingdata.o
	mkdir -p ./output/include ./output/lib
	cp src/thinkingdata.h ./output/include/.
	cp libthinkingdata.a ./output/lib/.

sdk-demo: thinkingdata.o json.o util.o list.o
	$(CC) -o $@ demo/demo.c $^ $(CFLAGS)

thinkingdata.o: src/thinkingdata.c src/thinkingdata.h src/thinkingdata_private.h 
	$(CC) -c src/thinkingdata.c $(CFLAGS)

list.o: src/list.c src/list.h 
	$(CC) -c src/list.c $(CFLAGS)

util.o: src/util.c src/util.h 
	$(CC) -c src/util.c $(CFLAGS)

json.o: src/json.c src/json.h 
	$(CC) -c src/json.c $(CFLAGS)

.PHONY: clean
	
clean:
	rm -rf *.o *.a
	rm -rf sdk-demo
	rm -rf output