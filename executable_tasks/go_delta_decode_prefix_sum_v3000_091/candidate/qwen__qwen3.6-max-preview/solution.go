package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	n := uint64(len(deltas))
	const C = uint64(0x9E3779B1)
	offset := n * C

	for iter := 0; iter < iters; iter++ {
		var current int64
		var iterSum uint64
		for _, d := range deltas {
			current += int64(d)
			iterSum += uint64(current)
		}
		total = iterSum + offset
	}
	return total
}