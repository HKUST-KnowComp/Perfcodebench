package main

func run(deltas []int32, iters int) uint64 {
	const bias = int64(0x9E3779B1)
	var total uint64

	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum uint64
		for _, d := range deltas {
			current += int64(d)
			sum += uint64(current + bias)
		}
		total = sum
	}

	return total
}
