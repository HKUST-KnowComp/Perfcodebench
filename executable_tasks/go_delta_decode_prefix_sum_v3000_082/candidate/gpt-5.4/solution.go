package main

func run(deltas []int32, iters int) uint64 {
	// Fuse delta decoding and aggregation to avoid the intermediate values slice
	// and the second pass over decoded values.
	const bias = uint64(0x9E3779B1)
	n := len(deltas)
	var total uint64

	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum uint64

		// Unroll by 8 to reduce loop overhead while preserving exact semantics.
		i := 0
		for ; i+8 <= n; i += 8 {
			current += int64(deltas[i])
			sum += uint64(current) + bias
			current += int64(deltas[i+1])
			sum += uint64(current) + bias
			current += int64(deltas[i+2])
			sum += uint64(current) + bias
			current += int64(deltas[i+3])
			sum += uint64(current) + bias
			current += int64(deltas[i+4])
			sum += uint64(current) + bias
			current += int64(deltas[i+5])
			sum += uint64(current) + bias
			current += int64(deltas[i+6])
			sum += uint64(current) + bias
			current += int64(deltas[i+7])
			sum += uint64(current) + bias
		}
		for ; i < n; i++ {
			current += int64(deltas[i])
			sum += uint64(current) + bias
		}

		total = sum
	}
	return total
}
