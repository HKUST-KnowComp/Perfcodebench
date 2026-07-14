package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	if iters <= 0 {
		return h
	}
	// Preallocate histogram once to avoid per-iteration heap allocation overhead
	hist := make([]uint64, int(buckets))
	for iter := 0; iter < iters; iter++ {
		// Fast runtime-optimized zeroing to replace per-iteration make()
		clear(hist)
		// Preserve original aggregation logic exactly
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		// Exact original checksum calculation to ensure correctness
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}