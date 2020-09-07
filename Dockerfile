FROM alpine

RUN apk add build-base libmicrohttpd libmicrohttpd-dev

COPY Makefile .
COPY router.c server.c shelf.c types.h vm.c ./
RUN make all
CMD ["./server"]
