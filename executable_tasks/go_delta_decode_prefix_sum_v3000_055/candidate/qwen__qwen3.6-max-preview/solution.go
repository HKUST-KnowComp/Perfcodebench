package main

func run(deltas []int32, iters int) uint64 {
	if iters <= 0 || len(deltas) == 0 {
		return 0
	}

	var total uint64
	weight := int64(len(deltas))

	// Transform sequential prefix-sum into a dependency-free weighted sum:
	// sum(prefix[i]) == sum(deltas[j] * (n - j))
	// This removes the loop-carried dependency, enabling ILP/vectorization.
	for _, d := range deltas {
		total += uint64(int64(d) * weight)
		weight--
	}

	// Add constant offset contribution (linear and modular-safe)
	total += uint64(len(deltas)) * uint64(0x9E3779B1)
	return total
}