#!/bin/sh

# This is a comment!

cwd=$(pwd)

cd ~/
if [ -d "Qt" ]; then
  cd Qt
  if [! -d "5.12.2" ]; then
    echo Install Qt 5.12.2 from Qt Maintenance Tool
  fi
fi

cd $cwd

cd server
if [ -d "build" ]; then
  # Take action if $DIR exists. #
  cd build
  echo Cleaning build folder...
  rm -R *
  echo Building the server..
  cmake ../ .
  make -j8
else
  echo Creating build folder...
  mkdir build
  cd build
  echo Building the server..
  cmake ../ .
  make -j8
fi

echo Done.
./ServerModule
