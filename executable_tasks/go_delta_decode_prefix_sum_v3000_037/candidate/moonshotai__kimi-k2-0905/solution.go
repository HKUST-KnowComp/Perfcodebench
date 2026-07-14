package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}

	const magic = 0x9E3779B1

	// Fast path: assume all deltas fit into int64 and the prefix never overflows.
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var cur int64
		var sum uint64

		// 8-way unrolled prefix-sum + constant add.
		i := 0
		for ; i+8 <= n; i += 8 {
			d0 := int64(deltas[i+0])
			d1 := int64(deltas[i+1])
			d2 := int64(deltas[i+2])
			d3 := int64(deltas[i+3])
			d4 := int64(deltas[i+4])
			d5 := int64(deltas[i+5])
			d6 := int64(deltas[i+6])
			d7 := int64(deltas[i+7])

			cur += d0
			sum += uint64(cur + magic)
			cur += d1
			sum += uint64(cur + magic)
			cur += d2
			sum += uint64(cur + magic)
			cur += d3
			sum += uint64(cur + magic)
			cur += d4
			sum += uint64(cur + magic)
			cur += d5
			sum += uint64(cur + magic)
			cur += d6
			sum += uint64(cur + magic)
			cur += d7
			sum += uint64(cur + magic)
		}

		// Tail.
		for ; i < n; i++ {
			cur += int64(deltas[i])
			sum += uint64(cur + magic)
		}

		total = sum
	}
	return total
}