#!/bin/bash

POLY="4  [4 0 0 0] [0 4 0 0] [0 0 4 0] [0 0 0 4] (2  0 2)[3 1 0 0] (2  0 7)[2 1 1 0] (2  0 -11)[1 2 1 0] (2  0 13)[0 2 1 1] (2  0 17)[0 2 0 2] (2  0 -1)[1 1 1 1]"
N=1
OUT=gmconnection-05

./gmconnection-p "$POLY" $N > $OUT

