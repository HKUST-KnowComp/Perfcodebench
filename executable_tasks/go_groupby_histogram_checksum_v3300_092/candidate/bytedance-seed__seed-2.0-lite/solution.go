package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	var h uint64
	if iters == 0 {
		return h
	}
	// Preallocate histogram once to avoid repeated heap allocations
	hist := make([]uint64, buckets)
	keyLen := len(keys)
	for iter := 0; iter < iters; iter++ {
		// Fast compiler-optimized slice zeroing
		clear(hist)
		// Hot aggregation loop with compiler-eliminated bounds checks
		for i := 0; i < keyLen; i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		// Exact original checksum logic preserved for correctness
		h = 1469598103934665603
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}