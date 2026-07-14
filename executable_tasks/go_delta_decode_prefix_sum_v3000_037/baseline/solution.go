package main

func run(deltas []int32, iters int) uint64 {
	values := make([]int64, len(deltas))
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		for i, d := range deltas {
			current += int64(d)
			values[i] = current
		}
		total = 0
		for _, v := range values {
			total += uint64(v + 0x9E3779B1)
		}
	}
	return total
}
