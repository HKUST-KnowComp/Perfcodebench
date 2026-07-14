package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}

	const constant = 0x9E3779B1
	var total uint64

	// Localize slice to help compiler with bounds check elimination
	d := deltas

	for iter := 0; iter < iters; iter++ {
		var current int64
		var subTotal uint64

		i := 0
		// Unroll the loop to reduce branch overhead and improve throughput
		for ; i <= n-8; i += 8 {
			c0 := current + int64(d[i])
			c1 := c0 + int64(d[i+1])
			c2 := c1 + int64(d[i+2])
			c3 := c2 + int64(d[i+3])
			c4 := c3 + int64(d[i+4])
			c5 := c4 + int64(d[i+5])
			c6 := c5 + int64(d[i+6])
			c7 := c6 + int64(d[i+7])

			subTotal += uint64(c0 + constant)
			subTotal += uint64(c1 + constant)
			subTotal += uint64(c2 + constant)
			subTotal += uint64(c3 + constant)
			subTotal += uint64(c4 + constant)
			subTotal += uint64(c5 + constant)
			subTotal += uint64(c6 + constant)
			subTotal += uint64(c7 + constant)
			current = c7
		}

		// Handle remaining elements
		for ; i < n; i++ {
			current += int64(d[i])
			subTotal += uint64(current + constant)
		}
		total = subTotal
	}

	return total
}