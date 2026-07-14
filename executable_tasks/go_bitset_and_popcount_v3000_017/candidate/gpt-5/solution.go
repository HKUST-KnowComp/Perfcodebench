package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	if iters <= 0 {
		return 0
	}

	n := len(left)
	var total uint64

	// Manual unrolling for better throughput
	i := 0
	for i+7 < n {
		total += uint64(bits.OnesCount64(left[i] & right[i]))
		total += uint64(bits.OnesCount64(left[i+1] & right[i+1]))
		total += uint64(bits.OnesCount64(left[i+2] & right[i+2]))
		total += uint64(bits.OnesCount64(left[i+3] & right[i+3]))
		total += uint64(bits.OnesCount64(left[i+4] & right[i+4]))
		total += uint64(bits.OnesCount64(left[i+5] & right[i+5]))
		total += uint64(bits.OnesCount64(left[i+6] & right[i+6]))
		total += uint64(bits.OnesCount64(left[i+7] & right[i+7]))
		i += 8
	}
	for i < n {
		total += uint64(bits.OnesCount64(left[i] & right[i]))
		i++
	}

	return total
}
