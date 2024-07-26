#FROM gcc:latest
#
#COPY . /build
#WORKDIR /build/
#
#RUN make all
#CMD ["./golang_runtime"]

FROM ubuntu:latest

RUN apt-get update && apt-get install -y valgrind && apt-get install -y software-properties-common && apt-get install -y gcc && apt-get install -y gdb && apt-get install -y vim && apt-get install -y build-essential 

COPY . /build
WORKDIR /build/

RUN make all
CMD ["./golang_runtime"]
#CMD ["valgrind", "--leak-check=yes", "./golang_runtime"]
#CMD ["valgrind", "--tool=memcheck", "./golang_runtime"]

#CMD ["valgrind", "--tool=helgrind", "--max-stackframe=10540336", "./golang_runtime"]

#CMD ["valgrind", "--tool=drd", "--read-var-info=yes", "./golang_runtime"] // data_race
#CMD ["valgrind", "--tool=drd", "--exclusive-threshold=10", "./golang_runtime"]
