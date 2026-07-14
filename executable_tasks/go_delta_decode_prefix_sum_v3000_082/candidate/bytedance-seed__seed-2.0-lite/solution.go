package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}
	offset := uint64(0x9E3779B1)
	offsetTotal := offset * uint64(n)
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		var iterSum uint64
		for _, d := range deltas {
			current += int64(d)
			iterSum += uint64(current)
		}
		total = iterSum + offsetTotal
	}
	return total
}