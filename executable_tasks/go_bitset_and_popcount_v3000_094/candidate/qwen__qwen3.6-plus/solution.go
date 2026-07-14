package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	for i := range left {
		total += bits.OnesCount64(left[i] & right[i])
	}
	return total
}