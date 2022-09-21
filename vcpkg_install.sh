#!/bin/bash
apt-get install -y build-essential curl zip unzip tar git
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh -disableMetrics
./vcpkg/vcpkg install poco
./vcpkg/vcpkg install libpq[openssl]
./vcpkg/vcpkg install libpqxx
./vcpkg/vcpkg install prometheus-cpp
./vcpkg/vcpkg install catch2:x64-linux

# Install custom lib that is not tracked in vcpkg
git clone https://github.com/WolvenSpirit/resource-pool
ln -s ${PWD}/resource-pool/resource-pool vcpkg/ports/resource-pool
./vcpkg/vcpkg install resource-pool:x64-linux