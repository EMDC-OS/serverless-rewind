#!/usr/bin/bash
NAMES=(
"6eb7417df64c4741b7417df64cc741c9"
"4bf2e2e02df441bdb2e2e02df401bd40"
"5e33212844e44f80b3212844e41f8068"
"9faebc09e815480baebc09e815a80b0c"
"273fc36b3e6c48f9bfc36b3e6c68f9b4"
"7f10c091b09742fa90c091b097c2fab5"
"8703fa6fde4242d283fa6fde4212d20f"
"94eb0803e0ec4064ab0803e0ecb06478"
"810e3583da9248a08e3583da92b8a05a"
"f11d916bc19241eb9d916bc19281eb95"
"2b2cf4d865414631acf4d86541d63114"
"66b2c19a4a6b4f50b2c19a4a6bff50a7"
)



for i in ${NAMES[@]}
do
 	echo `wsk activation get $i | grep end | head -n 1 | awk '{print $2}' | awk -F "," '{ print $1}'`"000000"
done
