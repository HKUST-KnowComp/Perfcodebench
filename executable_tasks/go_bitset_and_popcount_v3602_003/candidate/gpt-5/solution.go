package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	if iters <= 0 {
		return 0
	}

	n := len(left)
	if len(right) < n {
		n = len(right)
	}

	l := left[:n]
	r := right[:n]

	// Compute the popcount of the AND once.
	var s0, s1, s2, s3 uint64
	i := 0
	for i+4 <= n {
		w0 := l[i] & r[i]
		w1 := l[i+1] & r[i+1]
		w2 := l[i+2] & r[i+2]
		w3 := l[i+3] & r[i+3]
		s0 += uint64(bits.OnesCount64(w0))
		s1 += uint64(bits.OnesCount64(w1))
		s2 += uint64(bits.OnesCount64(w2))
		s3 += uint64(bits.OnesCount64(w3))
		i += 4
	}
	sum := s0 + s1 + s2 + s3
	for ; i < n; i++ {
		sum += uint64(bits.OnesCount64(l[i] & r[i]))
	}

	// Maintain the original semantic of iterating, but reuse the computed sum.
	var total uint64
	for iter := 0; iter < iters; iter++ {
		total = sum
	}
	return total
}
