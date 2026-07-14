package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// Pre-allocate one backing array reused across iterations.
	backing := make([]uint64, buckets)

	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Re-slice the reused backing array; zero only the needed buckets.
		hist := backing[:buckets]

		// Fast zeroing with 8× unrolled loop.
		n := int(buckets)
		for i := 0; i+7 < n; i += 8 {
			hist[i+0] = 0
			hist[i+1] = 0
			hist[i+2] = 0
			hist[i+3] = 0
			hist[i+4] = 0
			hist[i+5] = 0
			hist[i+6] = 0
			hist[i+7] = 0
		}
		for i := n &^ 7; i < n; i++ {
			hist[i] = 0
		}

		// Aggregation (same order as baseline).
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}

		// FNV-1a checksum.
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}