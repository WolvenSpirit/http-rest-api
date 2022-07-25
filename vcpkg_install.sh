#!/bin/bash
apt-get install -y build-essential curl zip unzip tar git
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh -disableMetrics
./vcpkg/vcpkg install poco
./vcpkg/vcpkg install libpq[openssl]
./vcpkg/vcpkg install libpqxx
./vcpkg/vcpkg install prometheus-cpp