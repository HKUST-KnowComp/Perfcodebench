package main

func run(deltas []int32, iters int) uint64 {
	const offset = 0x9E3779B1
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		if iter == iters-1 {
			var sum uint64
			for _, d := range deltas {
				current += int64(d)
				sum += uint64(current + offset)
			}
			total = sum
		} else {
			for _, d := range deltas {
				current += int64(d)
			}
		}
	}
	return total
}
