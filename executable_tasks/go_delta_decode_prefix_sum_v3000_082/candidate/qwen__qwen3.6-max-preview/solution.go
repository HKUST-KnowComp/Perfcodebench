package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	var total uint64
	offset := uint64(n) * 0x9E3779B1

	for iter := 0; iter < iters; iter++ {
		var acc int64
		weight := int64(n)
		for _, d := range deltas {
			acc += int64(d) * weight
			weight--
		}
		total = uint64(acc) + offset
	}
	return total
}