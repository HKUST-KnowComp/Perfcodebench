package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	// Allocate histogram once to avoid per-iteration heap allocation overhead
	hist := make([]uint64, buckets)
	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Fast optimized assembly-backed clear to reset buckets, equivalent to a new zeroed slice
		clear(hist)
		// Aggregate values exactly as baseline to preserve correctness
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		// Compute checksum with identical logic to baseline for exact matching
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}