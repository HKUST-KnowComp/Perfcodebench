package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}

	// Precompute the constant offset added to each prefix value
	offset := uint64(n) * 0x9E3779B1

	var total uint64
	for iter := 0; iter < iters; iter++ {
		// Fused single-pass: compute sum of prefix sums directly
		// prefix[i] = delta[0] + delta[1] + ... + delta[i]
		// sum(prefix[i]) = sum(delta[j] * (n - j)) for j in 0..n-1
		// We accumulate: current (running prefix) and prefixSum (sum of all prefixes so far)
		var current int64
		var prefixSum int64

		// Unroll by 4 for better ILP
		i := 0
		nn := n - 3
		for ; i < nn; i += 4 {
			d0 := int64(deltas[i])
			d1 := int64(deltas[i+1])
			d2 := int64(deltas[i+2])
			d3 := int64(deltas[i+3])

			p0 := current + d0
			p1 := p0 + d1
			p2 := p1 + d2
			p3 := p2 + d3

			prefixSum += p0 + p1 + p2 + p3
			current = p3
		}

		// Handle remainder
		for ; i < n; i++ {
			current += int64(deltas[i])
			prefixSum += current
		}

		total = uint64(prefixSum) + offset
	}
	return total
}
