package main

func run(deltas []int32, iters int) uint64 {
	if iters == 0 {
		return 0
	}
	var current int64
	var total uint64
	const magic = int64(0x9E3779B1)
	for i := 0; i < len(deltas); i++ {
		current += int64(deltas[i])
		total += uint64(current + magic)
	}
	return total
}
