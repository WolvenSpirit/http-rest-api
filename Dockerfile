FROM ubuntu:latest as build

COPY ./src ./src

COPY ./CMakeLists.txt ./CMakeLists.txt

COPY ./vcpkg_install.sh ./vcpkg_install.sh

RUN apt update

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get install -y cmake build-essential curl git pkg-config autoconf

RUN ./vcpkg_install.sh

# RUN ./build.sh
#RUN rm CMakeCache.txt

RUN cmake CMakeLists.txt "-DCMAKE_TOOLCHAIN_FILE=${PWD}\vcpkg\scripts\buildsystems\vcpkg.cmake"

RUN cmake --build .

RUN ls 

################################

FROM debian:stable-slim

COPY ./config.json ./config.json

COPY --from=build src ./src 
COPY --from=build main ./main

ENTRYPOINT [ "./main" ] 
#./main 9005

