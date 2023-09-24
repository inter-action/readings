#!/bin/bash



/usr/bin/clang++ -Wall -Werror -std=c++17 -fobjc-arc -c src/main.cpp -o build/main.o
echo "linking"
/usr/bin/clang++  -framework Foundation build/main.o -o build/app

build/app
# /usr/bin/clang++ -Wall -Werror -std=c++17 -framework Foundation -c src/main.cpp -o ./build/app
