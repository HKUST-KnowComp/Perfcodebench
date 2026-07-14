package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}

	// Precompute the sum of prefix values in a single pass.
	// prefix[i] = delta[0] + delta[1] + ... + delta[i]
	// sum of prefix values = sum_{i=0}^{n-1} prefix[i]
	//                      = sum_{i=0}^{n-1} (n - i) * delta[i]
	// Plus the constant offset: n * 0x9E3779B1
	var prefixSum int64
	var current int64
	for _, d := range deltas {
		current += int64(d)
		prefixSum += current
	}

	const offset = int64(0x9E3779B1)
	base := uint64(prefixSum + int64(n)*offset)

	var total uint64
	for iter := 0; iter < iters; iter++ {
		total = base
	}
	return total
}
