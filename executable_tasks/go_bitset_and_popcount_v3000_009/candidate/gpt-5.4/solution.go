package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var sum uint64
		for i, lw := range left {
			sum += uint64(bits.OnesCount64(lw & right[i]))
		}
		total = sum
	}
	return total
}
