package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	const offset = 0x9E3779B1
	for iter := 0; iter < iters; iter++ {
		var current int64
		for _, d := range deltas {
			current += int64(d)
			total += uint64(current + offset)
		}
	}
	return total
}