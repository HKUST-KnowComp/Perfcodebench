package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		var iterTotal uint64
		for i := 0; i < len(deltas); i++ {
			current += int64(deltas[i])
			iterTotal += uint64(current + 0x9E3779B1)
		}
		total = iterTotal
	}
	return total
}