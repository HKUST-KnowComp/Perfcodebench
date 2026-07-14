package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}

	// Precompute the constant offset added to each element
	// Each prefix value has 0x9E3779B1 added, so total offset = n * 0x9E3779B1
	offset := uint64(n) * 0x9E3779B1

	var total uint64
	for iter := 0; iter < iters; iter++ {
		// Fused single-pass: compute sum of prefix values directly
		// prefix[i] = sum(deltas[0..i])
		// sum of all prefix[i] = sum over i of (n-i)*delta[i]
		// We accumulate: current = running prefix, total = sum of all prefixes
		var current int64
		var prefixSum uint64
		for i := 0; i < n; i++ {
			current += int64(deltas[i])
			prefixSum += uint64(current)
		}
		total = prefixSum + offset
	}
	return total
}