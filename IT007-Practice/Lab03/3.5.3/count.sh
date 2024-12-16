#!/bin/bash

i=1
while [ $i -le $1 ]; do
    echo "Counting: $i"
    i=$((i + 1))
    sleep 1
done