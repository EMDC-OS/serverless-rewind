N=10
#LIST=("1" "2-1" "2-2" "2-3" "2-4" "3-1" "3-2" "4-1" "4-1-1" "4-2" "4-2-1" "4-3" "4-3-1" "5" "6" "6-1" "7" "8" "9")
#LIST=("1" "2-1" "2-2" "2-3" "2-4" "3-1" "3-2" "5" "6" "6-1" "7" "8")
LIST=("7" "9")


#rm /home/user01/paused.txt
#touch /home/user01/paused.txt

for WORK in ${LIST[@]}; do

# Update function
wsk action update --memory 4096 TEST${WORK}_fork TEST${WORK}_fork.zip --docker bskoon/rewind-eval:fork-debug
wsk action update --memory 4096 TEST${WORK}_rewind TEST${WORK}_rewind.zip --docker bskoon/rewind-eval:res-debug
wsk action update --memory 4096 TEST${WORK}_lib TEST${WORK}_lib.zip --docker bskoon/rewind-eval:lib-debug

echo "{" > ../results/result${WORK}.txt

echo "\"$WORK-fork\": {" >> ../results/result${WORK}.txt

wsk action invoke TEST${WORK}_fork --result >> /dev/null
sleep 2

echo "${WORK}_fork" > ../results/pf${WORK}.txt
echo -e "0: `cat /proc/vmstat | tail -n 9 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 8 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 7 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 6 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 5 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 4 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 3 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 2 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 1 | awk '{ print $2 }'`" >> ../results/pf${WORK}.txt

for i in $(seq 1 $N)
do
	echo "\"$i\": {" >> ../results/result${WORK}.txt
	echo "\"user_start\": `date +%s%N`," >> ../results/result${WORK}.txt 
	echo "\"result\": " >> ../results/result${WORK}.txt
	wsk action invoke TEST${WORK}_fork --result >> ../results/result${WORK}.txt
	echo "," >> ../results/result${WORK}.txt
	echo "\"user_end\": `date +%s%N`" >> ../results/result${WORK}.txt
	echo "}," >> ../results/result${WORK}.txt
	echo -e "$i: `cat /proc/vmstat | tail -n 9 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 8 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 7 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 6 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 5 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 4 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 3 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 2 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 1 | awk '{ print $2 }'`" >> ../results/pf${WORK}.txt
	sleep 2
done
truncate -s -2 ../results/result${WORK}.txt
echo "" >> ../results/result${WORK}.txt

echo "}," >> ../results/result${WORK}.txt

echo "$WORK-fork(pftime)" > ../results/kernel${WORK}.txt
dmesg | grep "REWIND(exit)" | tail -n 10 | awk '{ print $7 }' | sed ':a;N;$!ba;s/\n/ /g' >> ../results/kernel${WORK}.txt

echo "\"$WORK-lib\": {" >> ../results/result${WORK}.txt

wsk action invoke TEST${WORK}_lib --result >> /dev/null
sleep 2

echo "${WORK}_lib" >> ../results/pf${WORK}.txt
echo -e "0: `cat /proc/vmstat | tail -n 9 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 8 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 7 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 6 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 5 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 4 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 3 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 2 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 1 | awk '{ print $2 }'`" >> ../results/pf${WORK}.txt

for i in $(seq 1 $N)
do
	echo "\"$i\": {" >> ../results/result${WORK}.txt
	echo "\"user_start\": `date +%s%N`," >> ../results/result${WORK}.txt 
	echo "\"result\": " >> ../results/result${WORK}.txt
	wsk action invoke TEST${WORK}_lib --result >> ../results/result${WORK}.txt
	echo "," >> ../results/result${WORK}.txt
	echo "\"user_end\": `date +%s%N`" >> ../results/result${WORK}.txt
	echo "}," >> ../results/result${WORK}.txt
	echo -e "$i: `cat /proc/vmstat | tail -n 9 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 8 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 7 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 6 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 5 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 4 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 3 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 2 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 1 | awk '{ print $2 }'`" >> ../results/pf${WORK}.txt
	sleep 2
done
truncate -s -2 ../results/result${WORK}.txt
echo "" >> ../results/result${WORK}.txt

echo "}," >> ../results/result${WORK}.txt

echo "$WORK-lib(end_time)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(TIME)" | tail -n 11 | awk '{ print $7 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g' >> ../results/kernel${WORK}.txt
echo "$WORK-lib(pf_time)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(TIME)" | tail -n 11 | awk '{ print $12 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g' >> ../results/kernel${WORK}.txt
echo "$WORK-total(lib)" >> ../results/vma.txt
dmesg | grep "REWIND(alloc_vma)" | tail -n 10 | awk '{ print $4 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g' >> ../results/vma.txt
echo "$WORK-reuse(lib)" >> ../results/vma.txt
dmesg | grep "REWIND(alloc_vma)" | tail -n 10 | awk '{ print $5 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g' >> ../results/vma.txt

echo "\"$WORK-rewind\": {" >> ../results/result${WORK}.txt

wsk action invoke TEST${WORK}_rewind --result >> /dev/null
sleep 2

echo "${WORK}_rewind" >> ../results/chk.txt
dmesg | grep "REWIND(CHK): Takes" | tail -n 1 | awk '{ print $5 }' >> ../results/chk.txt
dmesg | grep "REWIND(CHK): Takes" | tail -n 1 | awk '{ print $8 }' >> ../results/chk.txt

echo "${WORK}_rewind" >> ../results/pf${WORK}.txt
echo -e "0: `cat /proc/vmstat | tail -n 9 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 8 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 7 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 6 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 5 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 4 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 3 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 2 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 1 | awk '{ print $2 }'`" >> ../results/pf${WORK}.txt

for i in $(seq 1 $N)
do
	echo "\"$i\": {" >> ../results/result${WORK}.txt
	echo "\"user_start\": `date +%s%N`," >> ../results/result${WORK}.txt 
	echo "\"result\": " >> ../results/result${WORK}.txt
	wsk action invoke TEST${WORK}_rewind --result >> ../results/result${WORK}.txt
	echo "," >> ../results/result${WORK}.txt
	echo "\"user_end\": `date +%s%N`" >> ../results/result${WORK}.txt
	echo "}," >> ../results/result${WORK}.txt
	echo -e "$i: `cat /proc/vmstat | tail -n 9 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 8 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 7 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 6 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 5 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 4 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 3 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 2 | head -n 1 | awk '{ print $2 }'`\t`cat /proc/vmstat | tail -n 1 | awk '{ print $2 }'`" >> ../results/pf${WORK}.txt	
	sleep 2
done
truncate -s -2 ../results/result${WORK}.txt
echo "" >> ../results/result${WORK}.txt

echo "$WORK-rewind(end_time)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(TIME)" | tail -n 11 | awk '{ print $7 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g' >> ../results/kernel${WORK}.txt
echo "$WORK-rewind(pf_time)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(TIME)" | tail -n 11 | awk '{ print $12 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g'  >> ../results/kernel${WORK}.txt
echo "$WORK-rewind(mm_end_time)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(RW): End" | tail -n 10 | awk '{ print $5 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g'  >> ../results/kernel${WORK}.txt
echo "$WORK-rewind(mm_total_time)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(RW): End" | tail -n 10 | awk '{ print $6 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g'  >> ../results/kernel${WORK}.txt
echo "$WORK-rewind(mm_unmap_time)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(RW): End" | tail -n 10 | awk '{ print $7 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g' >> ../results/kernel${WORK}.txt
echo "$WORK-rewind(mm_copy_time)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(RW): End" | tail -n 10 | awk '{ print $8 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g'  >> ../results/kernel${WORK}.txt
echo "$WORK-rewind(mm_clear_time)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(RW): End" | tail -n 10 | awk '{ print $9 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g' >> ../results/kernel${WORK}.txt
echo "$WORK-rewind(mm_flush_time)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(RW): End" | tail -n 10 | awk '{ print $10 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g'  >> ../results/kernel${WORK}.txt
echo "$WORK-rewind(clear_pages)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(RW): C" | tail -n 10 | awk '{ print $4 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g'  >> ../results/kernel${WORK}.txt
echo "$WORK-rewind(acces_ptes)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(RW): C" | tail -n 10 | awk '{ print $5 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g'  >> ../results/kernel${WORK}.txt
echo "$WORK-rewind(erase_pages)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(RW): C" | tail -n 10 | awk '{ print $6 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g'  >> ../results/kernel${WORK}.txt
echo "$WORK-rewind(cow_pages)" >> ../results/kernel${WORK}.txt
dmesg | grep "REWIND(RW): C" | tail -n 10 | awk '{ print $7 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g'  >> ../results/kernel${WORK}.txt

#echo "$WORK-total" >> ../results/vma${WORK}.txt
dmesg | grep "REWIND(vma)" | tail -n 10 | awk '{ print $4 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g' >> ../results/kernel${WORK}.txt
#echo "$WORK-rewind" >> ../results/vma${WORK}.txt
dmesg | grep "REWIND(vma)" | tail -n 10 | awk '{ print $5 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g'  >> ../results/kernel${WORK}.txt
#echo "$WORK-unmapped" >> ../results/vma${WORK}.txt
dmesg | grep "REWIND(vma)" | tail -n 10 | awk '{ print $6 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g'  >> ../results/kernel${WORK}.txt

echo "$WORK-total(rewind)" >> ../results/vma.txt
dmesg | grep "REWIND(alloc_vma)" | tail -n 20 | awk '{ print $4 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g' >> ../results/vma.txt
echo "$WORK-reuse(rewind)" >> ../results/vma.txt
dmesg | grep "REWIND(alloc_vma)" | tail -n 20 | awk '{ print $5 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' | sed ':a;N;$!ba;s/\n/ /g' >> ../results/vma.txt

#echo "$WORK-rewind(pgd)" >> ../results/ptw${WORK}.txt
#dmesg | grep "REIWND(ptw)" | tail -n 5 | awk '{ print $4 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' >> ../results/ptw${WORK}.txt
#echo "$WORK-rewind(p4d)" >> ../results/ptw${WORK}.txt
#dmesg | grep "REIWND(ptw)" | tail -n 5 | awk '{ print $5 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' >> ../results/ptw${WORK}.txt
#echo "$WORK-rewind(pud)" >> ../results/ptw${WORK}.txt
#dmesg | grep "REIWND(ptw)" | tail -n 5 | awk '{ print $6 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' >> ../results/ptw${WORK}.txt
#echo "$WORK-rewind(pmd)" >> ../results/ptw${WORK}.txt
#dmesg | grep "REIWND(ptw)" | tail -n 5 | awk '{ print $7 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' >> ../results/ptw${WORK}.txt
#echo "$WORK-rewind(pte)" >> ../results/ptw${WORK}.txt
#dmesg | grep "REIWND(ptw)" | tail -n 5 | awk '{ print $8 }' | awk -F "=" '{ print $2 }' | awk -F "," '{ print $1 }' >> ../results/ptw${WORK}.txt

echo "}" >> ../results/result${WORK}.txt

echo "}" >> ../results/result${WORK}.txt

#echo "----------------------------------" >> /home/user01/paused.txt

#echo "$WORK"
#dmesg | grep "REWIND(reuse)" | tail -n 11

done

#cp /home/user01/paused.txt /home/user01/rewind_workload/results/paused.txt
