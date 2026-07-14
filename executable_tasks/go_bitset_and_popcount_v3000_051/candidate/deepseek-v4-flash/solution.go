package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	pop := uint64(0)
	for i := range left {
		pop += uint64(bits.OnesCount64(left[i] & right[i]))
	}
	for iter := 0; iter < iters; iter++ {
		total = pop
	}
	return total
}