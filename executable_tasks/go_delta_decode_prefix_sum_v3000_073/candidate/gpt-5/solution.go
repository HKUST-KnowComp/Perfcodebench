package main

func run(deltas []int32, iters int) uint64 {
	const add int64 = 0x9E3779B1
	var total uint64
	// Repeat the computation iters times, returning the result of the last iteration
	for iter := 0; iter < iters; iter++ {
		var current int64
		total = 0
		n := len(deltas)
		i := 0
		// Unroll by 8 for better instruction throughput
		for i+7 < n {
			current += int64(deltas[i+0])
			total += uint64(current + add)
			current += int64(deltas[i+1])
			total += uint64(current + add)
			current += int64(deltas[i+2])
			total += uint64(current + add)
			current += int64(deltas[i+3])
			total += uint64(current + add)
			current += int64(deltas[i+4])
			total += uint64(current + add)
			current += int64(deltas[i+5])
			total += uint64(current + add)
			current += int64(deltas[i+6])
			total += uint64(current + add)
			current += int64(deltas[i+7])
			total += uint64(current + add)
			i += 8
		}
		for ; i < n; i++ {
			current += int64(deltas[i])
			total += uint64(current + add)
		}
	}
	return total
}
