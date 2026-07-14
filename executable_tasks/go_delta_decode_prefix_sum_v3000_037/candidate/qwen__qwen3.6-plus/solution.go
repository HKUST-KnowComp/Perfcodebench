package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	n := uint64(len(deltas))
	const C = 0x9E3779B1
	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum int64
		for _, d := range deltas {
			current += int64(d)
			sum += current
		}
		total = uint64(sum) + n*C
	}
	return total
}
