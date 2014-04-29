#!/bin/bash

for i in {18..25}
do
    if [ -f $i.log ] 
    then
	rm $i.log
    fi
    for j in {1..25}
    do
	./cmaesXD $i 20 | tee -a $i.log
    done
done
