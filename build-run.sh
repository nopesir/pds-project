#!/bin/sh

# This is a comment!
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
