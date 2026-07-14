package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	n := len(left)
	if n == 0 {
		return 0
	}
	// Match baseline assumption that len(right) >= len(left) and eliminate
	// per-iteration bounds checks on right.
	_ = right[n-1]

	var s0, s1, s2, s3 uint64
	i := 0
	for ; i+3 < n; i += 4 {
		s0 += uint64(bits.OnesCount64(left[i] & right[i]))
		s1 += uint64(bits.OnesCount64(left[i+1] & right[i+1]))
		s2 += uint64(bits.OnesCount64(left[i+2] & right[i+2]))
		s3 += uint64(bits.OnesCount64(left[i+3] & right[i+3]))
	}
	total := s0 + s1 + s2 + s3
	for ; i < n; i++ {
		total += uint64(bits.OnesCount64(left[i] & right[i]))
	}
	return total
}
