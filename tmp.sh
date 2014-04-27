#!/bin/bash

for i in {1..25}
do
    if [ -f $i.log ] 
    then
	rm $i.log
    fi
    for j in {1..25}
    do
	./cmaesXD $i 20
    done
done
