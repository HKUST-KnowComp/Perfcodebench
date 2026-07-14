package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	total := uint64(0)
	for i := range left {
		total += uint64(bits.OnesCount64(left[i] & right[i]))
	}
	return total
}
