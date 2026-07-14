package main

func run(deltas []int32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}

	var total uint64
	n := len(deltas)
	const offset int64 = 0x9E3779B1

	// The result of the calculation is deterministic based on the input deltas.
	// We perform the calculation 'iters' times as requested by the harness,
	// but optimize the inner loop for maximum throughput.
	for iter := 0; iter < iters; iter++ {
		var current int64
		var localTotal uint64
		i := 0

		// Unroll the loop by 4 to reduce overhead and improve ILP
		for ; i <= n-4; i += 4 {
			d0 := int64(deltas[i])
			d1 := int64(deltas[i+1])
			d2 := int64(deltas[i+2])
			d3 := int64(deltas[i+3])

			c0 := current + d0
			c1 := c0 + d1
			c2 := c1 + d2
			c3 := c2 + d3

			localTotal += uint64(c0 + offset)
			localTotal += uint64(c1 + offset)
			localTotal += uint64(c2 + offset)
			localTotal += uint64(c3 + offset)

			current = c3
		}

		// Handle remaining elements
		for ; i < n; i++ {
			current += int64(deltas[i])
			localTotal += uint64(current + offset)
		}
		total = localTotal
	}

	return total
}