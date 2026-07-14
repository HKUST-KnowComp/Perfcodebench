package main

func run(deltas []int32, iters int) uint64 {
	const c int64 = 0x9E3779B1
	n := len(deltas)
	var total uint64
	for iter := 0; iter < iters; iter++ {
		total = 0
		var cur int64

		// Unroll by 8 for better throughput
		n8 := n &^ 7
		for i := 0; i < n8; i += 8 {
			cur += int64(deltas[i+0])
			total += uint64(cur + c)
			cur += int64(deltas[i+1])
			total += uint64(cur + c)
			cur += int64(deltas[i+2])
			total += uint64(cur + c)
			cur += int64(deltas[i+3])
			total += uint64(cur + c)
			cur += int64(deltas[i+4])
			total += uint64(cur + c)
			cur += int64(deltas[i+5])
			total += uint64(cur + c)
			cur += int64(deltas[i+6])
			total += uint64(cur + c)
			cur += int64(deltas[i+7])
			total += uint64(cur + c)
		}

		for i := n8; i < n; i++ {
			cur += int64(deltas[i])
			total += uint64(cur + c)
		}
	}
	return total
}
