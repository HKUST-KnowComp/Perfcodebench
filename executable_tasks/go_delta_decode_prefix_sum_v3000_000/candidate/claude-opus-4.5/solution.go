package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}

	// Precompute the constant offset contribution
	offsetSum := uint64(n) * 0x9E3779B1

	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum0, sum1, sum2, sum3 int64

		// Process 4 elements at a time
		i := 0
		limit := n - 3
		for i < limit {
			d0 := int64(deltas[i])
			d1 := int64(deltas[i+1])
			d2 := int64(deltas[i+2])
			d3 := int64(deltas[i+3])

			p0 := current + d0
			p1 := p0 + d1
			p2 := p1 + d2
			p3 := p2 + d3

			sum0 += p0
			sum1 += p1
			sum2 += p2
			sum3 += p3

			current = p3
			i += 4
		}

		// Handle remaining elements
		var tailSum int64
		for ; i < n; i++ {
			current += int64(deltas[i])
			tailSum += current
		}

		total = uint64(sum0+sum1+sum2+sum3+tailSum) + offsetSum
	}
	return total
}
