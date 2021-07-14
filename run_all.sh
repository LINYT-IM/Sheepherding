# for i in $(seq 0.1 0.1 2.0) 
# do 
# 	for j in $(seq 0.1 0.1 2.0) 
# 	do
# 		echo $i $j
# 	done
# done

> result.txt
for i in $(seq 0.01 0.01 2.0)
do 
	for j in $(seq 0.01 0.01 2.0)
	do
		for k in $(seq 0.01 0.01 2.0)
		do
			echo $i $j $k
			./sheep.exe 5 10 $i $j $k >> result.txt
		done
	done
done 