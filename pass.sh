#! /bin/bash

clang++ pass/InstLoggerPass.cpp -c -fPIC -I`llvm-config --includedir` -o pass/InstLoggerPass.o
clang++ pass/InstLoggerPass.o -fPIC -shared -o pass/libPass.so
clang -lSDL2 -Xclang -load -Xclang pass/libPass.so app/sim.c app/app.c pass/log.c -flegacy-pass-manager
./a.out > stats.txt