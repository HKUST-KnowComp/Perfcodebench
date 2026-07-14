package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}

	const offset int64 = 0x9E3779B1
	totalOffset := uint64(offset) * uint64(n)

	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		var prefixSum int64
		for _, d := range deltas {
			current += int64(d)
			prefixSum += current
		}
		total = uint64(prefixSum) + totalOffset
	}
	return total
}