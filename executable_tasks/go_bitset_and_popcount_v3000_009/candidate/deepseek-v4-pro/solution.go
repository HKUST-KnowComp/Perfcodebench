package main

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		total = 0
		for i := range left {
			w := left[i] & right[i]
			// Population count (hack parallel)
			w = w - ((w >> 1) & 0x5555555555555555)
			w = (w & 0x3333333333333333) + ((w >> 2) & 0x3333333333333333)
			w = (w + (w >> 4)) & 0x0F0F0F0F0F0F0F0F
			total += (w * 0x0101010101010101) >> 56
		}
	}
	return total
}