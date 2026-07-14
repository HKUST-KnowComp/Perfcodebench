package main

func run(deltas []int32, iters int) uint64 {
	values := make([]int64, len(deltas))
	const k = 0x9E3779B1

	var total uint64
	for iter := 0; iter < iters; iter++ {
		var cur int64
		total = 0
		for i, d := range deltas {
			cur += int64(d)
			values[i] = cur
			total += uint64(cur + k)
		}
	}
	return total
}