package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	var sumPop uint64
	for i := 0; i < len(left); i++ {
		sumPop += uint64(bits.OnesCount64(left[i] & right[i]))
	}
	for i := 0; i < iters; i++ {
		total = sumPop
	}
	return total
}
