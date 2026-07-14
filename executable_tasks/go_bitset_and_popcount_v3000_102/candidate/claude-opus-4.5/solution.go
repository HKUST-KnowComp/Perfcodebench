package main

func run(left, right []uint64, iters int) uint64 {
	var total uint64
	n := len(left)
	for iter := 0; iter < iters; iter++ {
		total = 0
		for i := 0; i < n; i++ {
			word := left[i] & right[i]
			// popcount via divide-and-conquer
			word = word - ((word >> 1) & 0x5555555555555555)
			word = (word & 0x3333333333333333) + ((word >> 2) & 0x3333333333333333)
			word = (word + (word >> 4)) & 0x0f0f0f0f0f0f0f0f
			total += (word * 0x0101010101010101) >> 56
		}
	}
	return total
}