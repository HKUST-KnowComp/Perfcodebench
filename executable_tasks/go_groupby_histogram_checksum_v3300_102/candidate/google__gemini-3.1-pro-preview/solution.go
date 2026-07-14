package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	// Pre-allocate the histogram buffer outside the loop to reuse memory.
	hist := make([]uint64, buckets)

	for iter := 0; iter < iters; iter++ {
		// Zero the histogram efficiently (Go compiler optimizes this to memclr).
		for i := range hist {
			hist[i] = 0
		}

		// Localize slice lengths to help the compiler eliminate bounds checks.
		n := len(keys)
		if len(vals) < n {
			n = len(vals)
		}
		k := keys[:n]
		v := vals[:n]

		// Unroll the aggregation loop to improve throughput.
		i := 0
		for ; i <= n-4; i += 4 {
			k0, v0 := k[i], v[i]
			k1, v1 := k[i+1], v[i+1]
			k2, v2 := k[i+2], v[i+2]
			k3, v3 := k[i+3], v[i+3]

			hist[k0] += uint64(v0)
			hist[k1] += uint64(v1)
			hist[k2] += uint64(v2)
			hist[k3] += uint64(v3)
		}

		// Handle remaining elements.
		for ; i < n; i++ {
			hist[k[i]] += uint64(v[i])
		}

		// FNV-1a-like checksum calculation.
		h = 1469598103934665603
		for _, val := range hist {
			h ^= val + 1
			h *= 1099511628211
		}
	}

	return h
}