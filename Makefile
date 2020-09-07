start: build
	docker run -it -p 8080:8080 library

background:
	docker run -t -d -p 8080:8080 library

stop:
	docker kill `docker ps | grep library | cut -d' ' -f1`

build:
	docker build -t library .

PROG = library
SRC = obscura.c

all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(SRC) -o library -lmicrohttpd

clean:
	rm -f $(PROG)

test:
	bash test.sh

.PHONY: all start build
