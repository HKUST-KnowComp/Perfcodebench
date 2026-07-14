package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	deltaLen := len(deltas)
	for iter := 0; iter < iters; iter++ {
		var current int64
		total = 0
		for i := 0; i < deltaLen; i++ {
			current += int64(deltas[i])
			total += uint64(current + 0x9E3779B1)
		}
	}
	return total
}