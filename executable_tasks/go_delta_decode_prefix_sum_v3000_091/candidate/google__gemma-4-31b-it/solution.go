package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	n := int64(len(deltas))
	const offset = 0x9E3779B1

	// Precompute the sum of prefix values for one iteration
	// Sum_{i=0 to n-1} (PrefixSum_i + offset)
	// = (Sum_{i=0 to n-1} PrefixSum_i) + n * offset
	// PrefixSum_i = Sum_{j=0 to i} deltas[j]
	// Sum_{i=0 to n-1} PrefixSum_i = Sum_{j=0 to n-1} (deltas[j] * (n - j))

	var singleIterSum uint64
	var currentPrefix int64
	for _, d := range deltas {
		currentPrefix += int64(d)
		singleIterSum += uint64(currentPrefix + offset)
	}

	// The result is the same for every iteration
	return singleIterSum * uint64(iters)
}