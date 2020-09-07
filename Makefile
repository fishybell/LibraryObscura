start: build
	docker run -it -p 8080:8080 server

background:
	docker run -t -d -p 8080:8080 server

stop:
	docker kill `docker ps | grep server | cut -d' ' -f1`

build:
	docker build -t server .

PROG = server
SRC = obscura.c router.c server.c shelf.c vm.c

all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(SRC) -o server -lmicrohttpd

clean:
	rm -f $(PROG)

test:
	bash test.sh

.PHONY: all start build
