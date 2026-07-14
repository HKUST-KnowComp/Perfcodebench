package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	const magic = 0x9E3779B1
	for iter := 0; iter < iters; iter++ {
		var sum uint64
		var cur int64
		n := len(deltas)
		// 4-way unrolled single pass
		for i := 0; i+3 < n; i += 4 {
			d0 := int64(deltas[i])
			d1 := int64(deltas[i+1])
			d2 := int64(deltas[i+2])
			d3 := int64(deltas[i+3])
			cur += d0
			sum += uint64(cur) + magic
			cur += d1
			sum += uint64(cur) + magic
			cur += d2
			sum += uint64(cur) + magic
			cur += d3
			sum += uint64(cur) + magic
		}
		// handle remaining elements
		for i := n &^ 3; i < n; i++ {
			cur += int64(deltas[i])
			sum += uint64(cur) + magic
		}
		total = sum
	}
	return total
}