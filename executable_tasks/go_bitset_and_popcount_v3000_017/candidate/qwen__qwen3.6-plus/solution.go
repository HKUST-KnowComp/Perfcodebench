package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var c uint64
		for i := range left {
			c += uint64(bits.OnesCount64(left[i] & right[i]))
		}
		total = c
	}
	return total
}