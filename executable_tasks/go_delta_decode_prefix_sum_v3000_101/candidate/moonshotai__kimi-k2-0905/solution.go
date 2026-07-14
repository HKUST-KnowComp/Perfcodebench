package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		var acc0, acc1 uint64 // 128-bit accumulator
		for _, d := range deltas {
			current += int64(d)
			sum := uint64(current + 0x9E3779B1)
			acc0, acc1 = acc0+sum, acc1+(acc0+sum>>63) // carry propagation
		}
		total = acc0 + acc1<<63 // combine into single uint64
	}
	return total
}