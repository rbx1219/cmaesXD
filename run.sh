#!/bin/bash

read  begin
read  end

for (( i=$begin ; i<=$end ; i++))
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

