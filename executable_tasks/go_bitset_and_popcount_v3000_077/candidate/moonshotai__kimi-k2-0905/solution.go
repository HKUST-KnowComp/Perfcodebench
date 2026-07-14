package main

func run(left, right []uint64, iters int) uint64 {
	total := uint64(0)
	for iter := 0; iter < iters; iter++ {
		total = 0
		for i := range left {
			w := left[i] & right[i]
			// SWAR popcount (13 arithmetic ops, no branches)
			w -= (w >> 1) & 0x5555555555555555
			w = (w & 0x3333333333333333) + ((w >> 2) & 0x3333333333333333)
			w = (w + (w >> 4)) & 0x0f0f0f0f0f0f0f0f
			total += (w * 0x0101010101010101) >> 56
		}
	}
	return total
}