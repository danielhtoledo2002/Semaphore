default:
  just --list

init:
  cmake -S. -B cmake-build 

build: init
  cmake --build cmake-build

build_release: init 
  cmake --build cmake-build --config Release

run: build
  ./cmake-build/semaforo

run_release: build_release
  ./cmake-build/semaforo

clean:
  rm -rf cmake_build