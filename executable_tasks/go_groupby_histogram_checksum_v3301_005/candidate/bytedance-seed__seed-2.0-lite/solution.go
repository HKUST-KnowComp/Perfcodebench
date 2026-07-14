package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	// Preallocate histogram once to avoid repeated per-iteration heap allocations
	hist := make([]uint64, buckets)
	for iter := 0; iter < iters; iter++ {
		// Reset slice to zero using optimized compiler-builtin memset equivalent to baseline's new zeroed slice
		clear(hist)
		// Original unmodified aggregation logic to preserve correctness
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		// Exact same FNV-1a checksum calculation from baseline
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}