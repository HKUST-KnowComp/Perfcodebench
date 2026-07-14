package main

func run(deltas []int32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	var total uint64
	var current int64
	const addend int64 = 0x9E3779B1
	for _, d := range deltas {
		current += int64(d)
		total += uint64(current + addend)
	}
	return total
}
