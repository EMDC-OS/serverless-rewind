#LIST=("1" "2-1" "2-2" "2-3" "2-4" "3-1" "3-2" "4-1" "4-2" "4-3" "5" "6" "6-1" "7" "8" "9")
LIST=("4-1-1" "4-2-1" "4-3-1")
# Make zip file
#rm *.zip
for WORK in ${LIST[@]}; do
	echo $WORK
	cp ../workload/exec_${WORK}.py exec.py
	cp ../workload/handler_${WORK}.py handler.py
	zip -r TEST${WORK}_fork.zip exec.py
	zip -r TEST${WORK}_rewind.zip handler.py
	zip -r TEST${WORK}_lib.zip handler.py
done

