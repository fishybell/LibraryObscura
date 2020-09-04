start: build
	docker run -it -p 8080:8080 server

build:
	docker build -t server .

PROG = server
SRC = book.c router.c server.c shelf.c

all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(SRC) -o server -lmicrohttpd

clean:
	rm -f $(PROG)

.PHONY: all start build
