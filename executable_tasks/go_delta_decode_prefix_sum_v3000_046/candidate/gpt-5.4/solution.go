package main

func run(deltas []int32, iters int) uint64 {
	// Fuse delta decoding and aggregation to avoid the intermediate values slice
	// and the second pass over the data.
	const bias = uint64(0x9E3779B1)

	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum uint64

		// Index-based loop is typically a little easier for the compiler to
		// optimize than range when repeatedly converting element types.
		for i := 0; i < len(deltas); i++ {
			current += int64(deltas[i])
			sum += uint64(current) + bias
		}

		total = sum
	}
	return total
}
