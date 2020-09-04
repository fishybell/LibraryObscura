start: build
	docker run -it -p 8080:8080 server

build:
	docker build -t server .

PROG = server
SRC = router.c routes.c server.c

all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(SRC) -o server -lmicrohttpd

clean:
	rm -f $(PROG)

.PHONY: all start build
