package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current uint64
		for i := 0; i < len(left); i++ {
			current += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = current
	}
	return total
}