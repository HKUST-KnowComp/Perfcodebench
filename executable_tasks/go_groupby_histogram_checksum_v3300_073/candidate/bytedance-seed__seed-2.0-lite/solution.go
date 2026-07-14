package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if iters == 0 {
		return 0
	}
	// Preallocate histogram once to avoid repeated per-iteration allocations
	hist := make([]uint64, buckets)
	// Precompute unique keys from fixed input to only reset used buckets later
	seen := make([]bool, buckets)
	uniqueKeys := make([]uint32, 0, len(keys))
	for _, k := range keys {
		if !seen[k] {
			seen[k] = true
			uniqueKeys = append(uniqueKeys, k)
		}
	}

	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Accumulate values exactly as original implementation
		for i := range keys {
			hist[keys[i]] += uint64(vals[i])
		}
		// Compute FNV checksum using identical logic to preserve correctness
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
		// Reset only the buckets that were modified, not the entire slice
		for _, k := range uniqueKeys {
			hist[k] = 0
		}
	}
	return h
}