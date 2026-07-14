package main

import "math/bits"

func run(left, right []uint64, iters int) uint64 {
	// Use the minimum length to avoid bounds issues
	n := len(left)
	if len(right) < n {
		n = len(right)
	}
	l := left[:n]
	r := right[:n]

	var total uint64
	for iter := 0; iter < iters; iter++ {
		var t uint64
		i := 0
		// Unroll by 8 for better throughput
		for ; i+7 < n; i += 8 {
			t += uint64(bits.OnesCount64(l[i] & r[i]))
			t += uint64(bits.OnesCount64(l[i+1] & r[i+1]))
			t += uint64(bits.OnesCount64(l[i+2] & r[i+2]))
			t += uint64(bits.OnesCount64(l[i+3] & r[i+3]))
			t += uint64(bits.OnesCount64(l[i+4] & r[i+4]))
			t += uint64(bits.OnesCount64(l[i+5] & r[i+5]))
			t += uint64(bits.OnesCount64(l[i+6] & r[i+6]))
			t += uint64(bits.OnesCount64(l[i+7] & r[i+7]))
		}
		// Unroll by 4
		for ; i+3 < n; i += 4 {
			t += uint64(bits.OnesCount64(l[i] & r[i]))
			t += uint64(bits.OnesCount64(l[i+1] & r[i+1]))
			t += uint64(bits.OnesCount64(l[i+2] & r[i+2]))
			t += uint64(bits.OnesCount64(l[i+3] & r[i+3]))
		}
		// Tail
		for ; i < n; i++ {
			t += uint64(bits.OnesCount64(l[i] & r[i]))
		}
		total = t
	}
	return total
}
