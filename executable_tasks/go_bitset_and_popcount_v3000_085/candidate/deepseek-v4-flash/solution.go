package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	if iters == 0 {
		return 0
	}
	var total uint64
	for i := range left {
		total += uint64(bits.OnesCount64(left[i] & right[i]))
	}
	return total
}
