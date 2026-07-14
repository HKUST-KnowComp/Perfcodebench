package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	n := len(deltas)
	const C uint64 = 0x9E3779B1
	nC := uint64(n) * C
	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum uint64
		for _, d := range deltas {
			current += int64(d)
			sum += uint64(current)
		}
		total = sum + nC
	}
	return total
}