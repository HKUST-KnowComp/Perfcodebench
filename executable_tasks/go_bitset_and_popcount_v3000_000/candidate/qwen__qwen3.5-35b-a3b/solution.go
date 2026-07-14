package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		for i := range left {
			total += uint64(bits.Popcount(left[i] & right[i]))
		}
	}
	return total
}