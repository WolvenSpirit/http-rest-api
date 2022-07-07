rm CMakeCache.txt
apt install build-essential
cmake CMakeLists.txt "-DCMAKE_TOOLCHAIN_FILE=${PWD}\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build .