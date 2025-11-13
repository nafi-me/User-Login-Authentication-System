#!/bin/bash
gcc -c auth.c -o auth.o -lcrypto
g++ main.cpp auth.o -o userauth -lsqlite3 -lcrypto
echo "Build complete. Run ./userauth"
