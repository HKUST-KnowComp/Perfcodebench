package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	if len(left) == 0 {
		return 0
	}

	var total uint64
	for iter := 0; iter < iters; iter++ {
		iterTotal := uint64(0)
		for i := 0; i < len(left); i++ {
			iterTotal += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = iterTotal
	}
	return total
}