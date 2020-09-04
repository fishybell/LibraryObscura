FROM alpine

RUN apk add build-base libmicrohttpd libmicrohttpd-dev

COPY Makefile .
COPY server.c .
RUN make all
CMD ["./server"]
