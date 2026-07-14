package main

func run(deltas []int32, iters int) uint64 {
	if iters <= 0 || len(deltas) == 0 {
		return 0
	}
	var total uint64
	var current int64
	for i := 0; i < len(deltas); i++ {
		current += int64(deltas[i])
		total += uint64(current)
	}
	total += uint64(0x9E3779B1) * uint64(len(deltas))
	return total
}