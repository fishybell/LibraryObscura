start: build
	docker run -it -p 8080:8080 server

build:
	docker build -t server .

PROG = server
SRC = server.c

all: $(PROG)

$(PROG): $(SRC)
	$(CC) server.c -o server -lmicrohttpd

clean:
	rm -f $(PROG)

.PHONY: all start build
