package main

func run(left, right []uint64, iters int) uint64 {
	bitsArr := make([]byte, len(left)*64)
	var total uint64
	for iter := 0; iter < iters; iter++ {
		for i := range left {
			word := left[i] & right[i]
			base := i * 64
			for b := 0; b < 64; b++ {
				bitsArr[base+b] = byte((word >> uint(b)) & 1)
			}
		}
		total = 0
		for _, v := range bitsArr {
			total += uint64(v)
		}
	}
	return total
}
