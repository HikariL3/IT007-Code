#!/bin/sh
for file in $(ls f*.sh); do
   more $file
done
