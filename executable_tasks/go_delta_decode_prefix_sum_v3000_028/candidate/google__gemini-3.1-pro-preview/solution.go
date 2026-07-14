package main

func run(deltas []int32, iters int) uint64 {
	if len(deltas) == 0 {
		return 0
	}

	const K uint64 = 0x9E3779B1
	var total uint64
	n := len(deltas)

	for iter := 0; iter < iters; iter++ {
		var current int64
		var currentTotal uint64
		i := 0

		// Unroll the loop by 8 to reduce overhead and improve pipelining
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

			// Summing uint64(int64_a + int64_b) is equivalent to uint64(a) + uint64(b)
			// in two's complement 64-bit arithmetic.
			currentTotal += uint64(c0) + uint64(c1) + uint64(c2) + uint64(c3) +
				uint64(c4) + uint64(c5) + uint64(c6) + uint64(c7) + (8 * K)
			current = c7
		}

		// Handle remaining elements
		for ; i < n; i++ {
			current += int64(deltas[i])
			currentTotal += uint64(current) + K
		}
		total = currentTotal
	}

	return total
}
