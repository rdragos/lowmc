progname=${1:-reingold.x}
evaluations=${2:-0}

for j in {1..5}; do
    echo "Benchmarking $progname $evaluations, $j"
    # Extract the time printed from "Time2"
    echo "Running..."
	times[$j]=$(./$progname $evaluations 2>&1 | grep Time2 | tee -a logs/bench_$progname | awk '/Time2/ { print $3 }')
    #times[$i]=$(awk '/Time/ { print $3 }' $file)
    echo "time = ${times[$j]}"
done
# take average of 5 times
time=$(echo ${times[@]} | awk '{sum=0; for(i=1; i<=NF; i++){sum+=$i}; sum/=NF; print sum}')
echo $progname-$evaluations $time >> logs/results_$progname
