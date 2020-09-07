FROM alpine

RUN apk add build-base libmicrohttpd libmicrohttpd-dev

COPY Makefile .
COPY obscura.c ./
RUN make all
CMD ["./library"]
