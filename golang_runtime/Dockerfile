FROM gcc:latest

COPY . /build
WORKDIR /build/

RUN make all
CMD ["./golang_runtime"]