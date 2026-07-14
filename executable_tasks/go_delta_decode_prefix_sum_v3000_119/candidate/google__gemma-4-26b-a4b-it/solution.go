package main

func run(deltas []int32, iters int) uint64 {
	if len(deltas) == 0 {
		return 0
	}

	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		var iterTotal uint64
		for _, d := range deltas {
			current += int64(d)
			iterTotal += uint64(current + 0x9E3779B1)
		}
		total = iterTotal
	}
	return total
}