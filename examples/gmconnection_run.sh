#!/bin/bash

ARRAY=("3  [3 0 0] [0 3 0] [0 0 3] (2  0 1)[1 1 1]" \
       "3  [3 0 0] [0 3 0] [0 0 3] (2  0 1)[2 1 0] (2  0 1)[0 2 1] (2  0 1)[1 0 2]" \
       "3  (2  1 1)[3 0 0] [0 3 0] [0 0 3]" \
       "3  (2  1 1)[3 0 0] (2  1 1)[0 3 0] [0 0 3]" \
       "3  (2  1 1)[3 0 0] (2  1 1)[0 3 0] (2  1 1)[0 0 3]" \
       "3  [3 0 0] [0 3 0] [0 0 3] (2  0 314)[2 1 0] (2  0 42)[0 2 1] (2  0 271)[1 0 2] (2  0 -23)[1 1 1]" \
       "3  [3 0 0] [0 3 0] [0 0 3] (2  0 1)[2 1 0]")

OUT=("OUT-3-3-1" \
     "OUT-3-3-2" \
     "OUT-3-3-3" \
     "OUT-3-3-4" \
     "OUT-3-3-5" \
     "OUT-3-3-6" \
     "OUT-3-3-7")

COUNT=${#ARRAY[@]}

for ((i = 0; i < $COUNT; i++)); do
    ./gmconnection "${ARRAY[${i}]}" > ${OUT[${i}]}
done
