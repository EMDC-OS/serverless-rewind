#!/usr/bin/bash

# number_of_request = Iter * 10
Iter=19

date +%s%N >> thr$1_$2.txt

for i in $(seq 1 $Iter)
do
wsk action invoke TEST$1_$2
done

wsk action invoke TEST$1_$2 --result 2>> ids.txt 

date +%s%N >> thr$1_$2.txt

