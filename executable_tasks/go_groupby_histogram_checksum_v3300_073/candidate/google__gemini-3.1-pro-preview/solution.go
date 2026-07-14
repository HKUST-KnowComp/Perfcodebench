package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if buckets == 0 {
		return 0
	}

	// Move allocation outside the loop to reuse memory across iterations.
	hist := make([]uint64, buckets)
	var h uint64

	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	for iter := 0; iter < iters; iter++ {
		// Reset the histogram efficiently.
		for i := range hist {
			hist[i] = 0
		}

		// Local references to slices can help the compiler optimize bounds checks.
		k := keys
		v := vals
		n := len(k)

		// Unroll the aggregation loop to improve throughput.
		i := 0
		for ; i <= n-4; i += 4 {
			k0, k1, k2, k3 := k[i], k[i+1], k[i+2], k[i+3]
			v0, v1, v2, v3 := v[i], v[i+1], v[i+2], v[i+3]
			hist[k0] += uint64(v0)
			hist[k1] += uint64(v1)
			hist[k2] += uint64(v2)
			hist[k3] += uint64(v3)
		}
		// Handle remaining elements.
		for ; i < n; i++ {
			hist[k[i]] += uint64(v[i])
		}

		// Checksum calculation (FNV-1a style).
		// This part is inherently sequential.
		h = fnvOffset
		for _, val := range hist {
			h ^= val + 1
			h *= fnvPrime
		}
	}

	return h
}