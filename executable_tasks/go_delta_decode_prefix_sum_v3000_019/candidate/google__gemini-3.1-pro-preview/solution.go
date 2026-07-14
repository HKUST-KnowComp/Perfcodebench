package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}

	const offset uint64 = 0x9E3779B1
	// Pre-calculate the total contribution of the constant offset
	offsetTotal := uint64(n) * offset

	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		var acc int64
		i := 0

		// Unroll the loop to process 8 elements at a time
		for ; i <= n-8; i += 8 {
			d0 := int64(deltas[i])
			d1 := int64(deltas[i+1])
			d2 := int64(deltas[i+2])
			d3 := int64(deltas[i+3])
			d4 := int64(deltas[i+4])
			d5 := int64(deltas[i+5])
			d6 := int64(deltas[i+6])
			d7 := int64(deltas[i+7])

			c0 := current + d0
			c1 := c0 + d1
			c2 := c1 + d2
			c3 := c2 + d3
			c4 := c3 + d4
			c5 := c4 + d5
			c6 := c5 + d6
			c7 := c6 + d7

			acc += (c0 + c1) + (c2 + c3) + (c4 + c5) + (c6 + c7)
			current = c7
		}

		// Handle remaining elements
		for ; i < n; i++ {
			current += int64(deltas[i])
			acc += current
		}

		// Combine the accumulated prefix sums with the pre-calculated offset total
		total = uint64(acc) + offsetTotal
	}

	return total
}