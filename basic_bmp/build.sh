#!/bin/bash

set -xe

gcc -Wall -Wextra -o basic_bmp_main.out basic_bmp_main.c 
./basic_bmp_main.out

gcc -Wall -Wextra -o blur blur.c
# ./blur

