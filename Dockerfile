FROM alpine

RUN apk add build-base libmicrohttpd libmicrohttpd-dev

COPY Makefile .
COPY router.c routes.c server.c types.h ./
RUN make all
CMD ["./server"]
