package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	var count uint64
	for i := range left {
		count += uint64(bits.OnesCount64(left[i] & right[i]))
	}
	return count * uint64(iters)
}