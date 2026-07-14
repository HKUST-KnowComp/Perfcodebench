package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	n := len(deltas)
	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum uint64
		for i := 0; i < n; i++ {
			current += int64(deltas[i])
			sum += uint64(current + 0x9E3779B1)
		}
		total = sum
	}
	return total
}