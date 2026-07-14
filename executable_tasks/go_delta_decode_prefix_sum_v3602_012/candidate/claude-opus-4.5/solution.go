package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}

	var total uint64
	const offset int64 = 0x9E3779B1
	baseOffset := uint64(n) * uint64(offset)

	for iter := 0; iter < iters; iter++ {
		var prefixSum int64
		var accumulator int64

		i := 0
		limit := n - 3

		// Unrolled loop (4x)
		for ; i < limit; i += 4 {
			d0 := int64(deltas[i])
			d1 := int64(deltas[i+1])
			d2 := int64(deltas[i+2])
			d3 := int64(deltas[i+3])

			prefixSum += d0
			accumulator += prefixSum

			prefixSum += d1
			accumulator += prefixSum

			prefixSum += d2
			accumulator += prefixSum

			prefixSum += d3
			accumulator += prefixSum
		}

		// Handle remaining elements
		for ; i < n; i++ {
			prefixSum += int64(deltas[i])
			accumulator += prefixSum
		}

		total = uint64(accumulator) + baseOffset
	}

	return total
}