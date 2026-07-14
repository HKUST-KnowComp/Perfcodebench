package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	hist := make([]uint64, buckets)
	
	// Pre-calculate constants for the FNV-1a like hash
	const offsetBasis = 1469598103934665603
	const prime = 1099511628211

	for iter := 0; iter < iters; iter++ {
		// Reset histogram without re-allocating
		for i := range hist {
			hist[i] = 0
		}

		// Aggregate values
		for i := 0; i < len(keys); i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		// Calculate checksum
		h = offsetBasis
		for i := 0; i < int(buckets); i++ {
			h ^= hist[i] + 1
			h *= prime
		}
	}
	return h
}