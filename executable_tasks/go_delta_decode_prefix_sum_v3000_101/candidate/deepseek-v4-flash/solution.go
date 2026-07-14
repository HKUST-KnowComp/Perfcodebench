package main

func run(deltas []int32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	const C = 0x9E3779B1
	var cum int64
	var total uint64
	for _, d := range deltas {
		cum += int64(d)
		total += uint64(cum + C)
	}
	return total
}
