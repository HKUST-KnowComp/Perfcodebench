package main

func run(deltas []int32, iters int) uint64 {
	const C = uint64(0x9E3779B1)
	var finalTotal uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		var iterTotal uint64
		for _, d := range deltas {
			current += int64(d)
			iterTotal += uint64(current) + C
		}
		finalTotal = iterTotal
	}
	return finalTotal
}