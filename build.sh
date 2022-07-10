rm CMakeCache.txt
cmake CMakeLists.txt "-DCMAKE_TOOLCHAIN_FILE=${PWD}\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build .