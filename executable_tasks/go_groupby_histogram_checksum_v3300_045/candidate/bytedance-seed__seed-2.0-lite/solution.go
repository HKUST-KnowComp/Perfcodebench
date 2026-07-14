package main

import "slices"

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	// Allocate histogram once to avoid per-iteration allocation overhead
	hist := make([]uint64, buckets)
	for iter := 0; iter < iters; iter++ {
		// Efficiently reset all buckets to zero with runtime-optimized memclr
		slices.Clear(hist)
		// Preserve original aggregation logic to maintain correct bucket sums
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		// Exact same FNV-1a checksum calculation as baseline to guarantee matching output
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}