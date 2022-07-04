FROM ubuntu:latest as build

COPY . .

RUN apt update

RUN apt-get install -y cmake

RUN ./vcpkg_install.sh

RUN ./build.sh

################################

FROM debian:stable-slim

COPY --from=build main .

RUN ./main 9005

