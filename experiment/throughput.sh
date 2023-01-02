#!/usr/bin/bash
WORK=$1
NAME=$2
docker=$3
N=$4

#usage: ./throughput.sh 1 rewind res 7
#LIST=("1" "2-1" "2-2" "2-3" "2-4" "3-1" "3-2" "4-1" "4-2" "4-3" "5" "6" "6-1" "7" "8" "9")

echo "id a b c d e f g h i j k TEST${WORK}_${NAME}" >> ids.txt
# Update all functions -> Modified to test dataset
wsk action update --memory 4096 TEST${WORK}_${NAME} TEST${WORK}_${NAME}.zip --docker bskoon/rewind-eval:${docker}
#wsk action update TEST${WORK}_rewind TEST${WORK}_rewind.zip --docker bskoon/rewind-eval:res
#wsk action update TEST${WORK}_lib TEST${WORK}_lib.zip --docker bskoon/rewind-eval:lib


# First, warm up!
for i in $(seq 1 $N); do
wsk action invoke TEST${WORK}_${NAME} > /dev/null
done
wsk action invoke TEST${WORK}_${NAME} --result > /dev/null

#date +%s%N > throughput${WORK}.txt

# Shooting
taskset -c 1 ./throughput_node.sh $WORK $NAME &
taskset -c 3 ./throughput_node.sh $WORK $NAME &
taskset -c 5 ./throughput_node.sh $WORK $NAME &
taskset -c 7 ./throughput_node.sh $WORK $NAME &
taskset -c 9 ./throughput_node.sh $WORK $NAME &
taskset -c 11 ./throughput_node.sh $WORK $NAME &
taskset -c 13 ./throughput_node.sh $WORK $NAME &
taskset -c 15 ./throughput_node.sh $WORK $NAME &
taskset -c 17 ./throughput_node.sh $WORK $NAME &
taskset -c 19 ./throughput_node.sh $WORK $NAME &
taskset -c 21 ./throughput_node.sh $WORK $NAME &
taskset -c 23 ./throughput_node.sh $WORK $NAME &

