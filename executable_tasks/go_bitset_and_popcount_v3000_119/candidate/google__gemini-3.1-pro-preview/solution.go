package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	if len(left) == 0 || iters == 0 {
		return 0
	}
	_ = right[len(left)-1]
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var sum uint64
		for i := 0; i < len(left); i++ {
			sum += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = sum
	}
	return total
}
