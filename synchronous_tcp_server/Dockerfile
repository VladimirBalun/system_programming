FROM gcc:latest

COPY . /build
WORKDIR /build/

RUN make all
CMD ["./tcp_server"]